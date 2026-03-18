#pragma once
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include "../models/Account.hpp"
#include "../models/SavingsAccount.hpp"
#include "../models/CurrentAccount.hpp"
#include "../infrastructure/AuditLogs.hpp"

class AccountService {
private:
    std::vector<std::shared_ptr<Account>> accounts;
    AuditLogs& auditLogs;
    int nextId = 1000;

    std::string generateId(const std::string& prefix) {
        return prefix + std::to_string(++nextId);
    }

public:
    explicit AccountService(AuditLogs& audit) : auditLogs(audit) {}

    std::shared_ptr<SavingsAccount>
    openSavingsAccount(const Customer& customer, double initialDeposit) {
        std::string id  = generateId("SAV-");
        auto account    = std::make_shared<SavingsAccount>(id, initialDeposit, customer);
        accounts.push_back(account);
        auditLogs.log("OPENED SavingsAccount " + id +
                      " for " + customer.getName());
        std::cout << "[AccountService] Opened: " << account->toString() << "\n";
        return account;
    }

    std::shared_ptr<CurrentAccount>
    openCurrentAccount(const Customer& customer,
                       double initialDeposit,
                       double overdraft = 1000.0) {
        std::string id  = generateId("CUR-");
        auto account    = std::make_shared<CurrentAccount>(id, initialDeposit,
                                                           customer, overdraft);
        accounts.push_back(account);
        auditLogs.log("OPENED CurrentAccount " + id +
                      " for " + customer.getName());
        std::cout << "[AccountService] Opened: " << account->toString() << "\n";
        return account;
    }

    std::shared_ptr<Account> findById(const std::string& id) const {
        for (const auto& acc : accounts)
            if (acc->getAccountId() == id) return acc;
        throw std::runtime_error("Account not found: " + id);
    }

    void listAll() const {
        std::cout << "\n===== All Accounts =====\n";
        for (const auto& acc : accounts)
            std::cout << acc->toString() << "\n";
        std::cout << "========================\n";
    }
};