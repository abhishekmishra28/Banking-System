#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <vector>

#include "../models/Account.hpp"
#include "../models/CurrentAccount.hpp"
#include "../models/SavingAccount.hpp"
#include "../infrastructure/AuditLogs.hpp"

class AccountService {
private:
    AuditLogs& audit;
    int nextID = 100001;
    std::vector<shared_ptr<Account>> accounts;

    std::string generate_ID(std::string& prefix){
        return prefix + to_string(++nextID);
    }

public:
    explicit AccountService(AuditLogs& log) : audit(log) {}

    std::shared_ptr<savingAccount> openSavingAccount(const Customer& cust, double initialDeposit) {
        std::string id = generate_ID("SAV-");
        auto account = std::make_shared<savingAccount>(id,initialDeposit,cust);
        accounts.push_back(account);
        audit.log("Saving Account created with ID " + id + " for " + cust.getName());
        std::cout<<"[Account Services] : Saving Account Opened : "<<account->toString()<<"\n";
        return account;
    }
    shared_ptr<currentAccount> openCurrentAccount(const Customer& cust, double initialDeposit, const double overDraft = 1000.0){
        std::string id = generate_ID("CUR-");
        auto account = std::make_shared<currentAccount>(id,initialDeposit,cust);
        accounts.push_back(account);
        audit.log("Current Account created with ID " + id + " for " + cust.getName());
        std::cout<<"[Account Services] : Current Account Opened : "<<account->toString()<<"\n";
        return account;
    }

    std::shared_ptr<Account> findIdBy(const std::string id) const {
        for(const auto acc : accounts){
            if(acc->getAccountId() == id) return acc;
        }
        throw std::runtime_error("Account Not Found : " + id);
    }    

    void listAll() const {
        std::cout << "=========== Accounts List ============"<<std::endl;
        int cnt = 0;
        for(const auto acc : accounts) {
            std::cout<< ++cnt <<". "<<acc->toString()<<std::endl;
        }
        std::cout<<"\n============End List================\n";
    }
}