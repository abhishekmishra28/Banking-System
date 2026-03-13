#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include "NotificationService.hpp"
#include "../infrastructure/AuditLogs.hpp"
#include "../models/Account.hpp"
#include "../security/FraudDetectionService.hpp"

class TransactionService {
private:
    AuditLogs& auditLogs;
    NotificationService& notifService;
    FraudDetectionService& fraudService;
public:
    TransactionService (FraudDetectionService& fraud,
                        NotificationService& notif,
                        AuditLogs& audit) : fraudService(fraud), notifService(notif), auditLogs(audit) {}

    void deposit(Account& account, double amt){
        account.deposit(amt);  
        TransactionRecord txn{account.getAccountId(),amt,"DEPOSIT",std::chrono::system_clock::now()};
        bool flag = fraudService.analyze(txn);
        notifService.sendTransactionAlert(account,"DEPOSIT",amt);
        if(flag) notifService.sendFraudAlert(account);
        auditLogs.log("DEPOSIT : Rs."+ std::to_string(amt)+" to "+"account "+account.getAccountId()+"\n");
    }

    void withdraw(Account& account,double amt){
        account.withdraw(amt);
        TransactionRecord txn{account.getAccountId(),amt,"WITHDRAWAL",std::chrono::system_clock::now()};
        bool flag = fraudService.analyze(txn);
        notifService.sendTransactionAlert(account,"WITHDRAWAL",amt);
        if(flag) notifService.sendFraudAlert(account);
        auditLogs.log("WITHDRAWAL : Rs."+ std::to_string(amt)+" from account "+account.getAccountId()+"\n");
    }
    void transfer(Account& source, Account& target, double amt){
        source.transfer(amt,target);
        TransactionRecord txn {source.getAccountId(),amt, "TRANSFER",std::chrono::system_clock::now()};
        bool flag = fraudService.analyze(txn);
        notifService.sendTransactionAlert(source,"TRANSFER",amt);
        if(flag) notifService.sendFraudAlert(source);
        auditLogs.log("Transferred : Rs."+ std::to_string(amt)+" from account "+source.getAccountId() + " to account " + target.getAccountId()+"\n");
    }
};