#pragma once
#include <string>
#include <iostream>
#include "../models/Account.hpp"

class NotificationService {
public:
    void sendTransactionAlert(const Account& account,
                               const std::string& type,
                               double amount) {
        std::cout << "[NOTIFICATION] Email to " << account.getOwner().getEmail()
                  << " - " << type << " of $" << amount
                  << " on account " << account.getAccountId()
                  << " | New balance: $" << account.getBalance() << "\n";
    }

    void sendFraudAlert(const Account& account) {
        std::cout << "[FRAUD NOTIFICATION] SMS to " << account.getOwner().getPhone()
                  << " - Suspicious activity detected on account "
                  << account.getAccountId() << ". Please verify.\n";
    }

    void sendLoanDecision(const Customer& customer,
                          double amount,
                          bool approved,
                          double rate = 0.0) {
        if (approved)
            std::cout << "[LOAN NOTIFICATION] " << customer.getName()
                      << " - Loan of $" << amount
                      << " APPROVED at " << (rate * 100) << "% interest.\n";
        else
            std::cout << "[LOAN NOTIFICATION] " << customer.getName()
                      << " - Loan of $" << amount << " DECLINED.\n";
    }
};