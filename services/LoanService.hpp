#pragma once
#include <iostream>
#include <string>
#include "../models/Account.hpp"
#include "NotificationService.hpp"
#include "../infrastructure/AuditLogs.hpp"

struct LoanApplication {
    std::string applicationId;
    Customer    applicant;
    double      requestedAmount;
    int         termMonths;
};

class LoanService {
private:
    double maxLoanMultiplier = 5.0;   // Max loan = 5× balance
    double baseInterestRate  = 0.07;  // 7 %

    NotificationService& notifService;
    AuditLogs&           auditLogs;

public:
    LoanService(NotificationService& notif, AuditLogs& audit)
        : notifService(notif), auditLogs(audit) {}

    void processLoan(const LoanApplication& app, Account& account) {
        double maxAllowed = account.getBalance() * maxLoanMultiplier;
        bool   approved   = (app.requestedAmount <= maxAllowed);

        std::cout << "\n--- Loan Application: " << app.applicationId << " ---\n";
        std::cout << "Applicant      : " << app.applicant.getName() << "\n";
        std::cout << "Requested      : $" << app.requestedAmount << "\n";
        std::cout << "Max Eligible   : $" << maxAllowed << "\n";
        std::cout << "Decision       : " << (approved ? "APPROVED" : "DECLINED") << "\n";

        if (approved) {
            account.deposit(app.requestedAmount);
            std::cout << "Loan disbursed to account " << account.getAccountId() << "\n";
        }

        notifService.sendLoanDecision(app.applicant, app.requestedAmount,
                                      approved, baseInterestRate);
        auditLogs.log("LOAN " + std::string(approved ? "APPROVED" : "DECLINED") +
                      " $" + std::to_string(app.requestedAmount) +
                      " for " + app.applicant.getName());
    }
};