#pragma once
#include <string>
#include <vector>
#include <chrono>
#include "../models/Account.hpp"
#include "../security/FraudDetectionService.hpp"
#include "NotificationService.hpp"
#include "../infrastructure/AuditLogs.hpp"

class TransactionService {
private:
    FraudDetectionService& fraudService;
    NotificationService&   notifService;
    AuditLogs&             auditLogs;

public:
    TransactionService(FraudDetectionService& fraud,
                       NotificationService& notif,
                       AuditLogs& audit)
        : fraudService(fraud), notifService(notif), auditLogs(audit) {}

    void deposit(Account& account, double amount) {
        account.deposit(amount);

        TransactionRecord txn{ account.getAccountId(), amount, "DEPOSIT",
                               std::chrono::system_clock::now() };
        bool flagged = fraudService.analyse(txn);

        notifService.sendTransactionAlert(account, "DEPOSIT", amount);
        if (flagged) notifService.sendFraudAlert(account);

        auditLogs.log("DEPOSIT $" + std::to_string(amount) +
                      " to " + account.getAccountId());
    }

    void withdraw(Account& account, double amount) {
        account.withdraw(amount);

        TransactionRecord txn{ account.getAccountId(), amount, "WITHDRAWAL",
                               std::chrono::system_clock::now() };
        bool flagged = fraudService.analyse(txn);

        notifService.sendTransactionAlert(account, "WITHDRAWAL", amount);
        if (flagged) notifService.sendFraudAlert(account);

        auditLogs.log("WITHDRAWAL $" + std::to_string(amount) +
                      " from " + account.getAccountId());
    }

    void transfer(Account& source, Account& target, double amount) {
        source.transfer(amount, target);

        TransactionRecord txn{ source.getAccountId(), amount, "TRANSFER",
                               std::chrono::system_clock::now() };
        bool flagged = fraudService.analyse(txn);

        notifService.sendTransactionAlert(source, "TRANSFER OUT", amount);
        notifService.sendTransactionAlert(target, "TRANSFER IN",  amount);
        if (flagged) notifService.sendFraudAlert(source);

        auditLogs.log("TRANSFER $" + std::to_string(amount) +
                      " from " + source.getAccountId() +
                      " to "   + target.getAccountId());
    }
};