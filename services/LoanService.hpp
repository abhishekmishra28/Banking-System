#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include "NotificationService.hpp"
#include "../models/Account.hpp"
#include "../infrastructure/AuditLogs.hpp"

struct LoanApplication {
    std::string& applicationId;
    Customer& applicant;
    double loanAmount;
    int months;
};

class LoanService {
private:
    NotificationService& notify;
    AuditLogs& log;
    double maxLoanMultiplier = 5.0;
    double baseIntrest = 0.07;
public:
    LoanService(NotificationService& notif, AuditLogs& audi)
        : notify(notif),log(audi) {}
    void processLoan(const LoanApplication& app, Account& acc){
        double maxAllowed = maxLoanMultiplier * acc.getBalance();
        bool approaved = (app.loanAmount <= maxAllowed);
        std::cout << "\n--- Loan Application: " << app.applicationId << " ---\n";
        std::cout << "Applicant      : " << app.applicant.getName() << "\n";
        std::cout << "Requested      : $" << app.loanAmount << "\n";
        std::cout << "Max Eligible   : $" << maxAllowed << "\n";
        std::cout << "Decision       : " << (approved ? "APPROVED" : "DECLINED") << "\n";

        if(approaved) {
            acc.deposit(app.loanAmount);
            std::cout << "Loan Disbursed to account "<< acc.getAccountId()<<std::endl;
        }
        notify.sendLoanDecision(acc,app.loanAmount,approaved,baseIntrest);
        log.log("LOAN " + std::string(approved ? "APPROVED" : "DECLINED") +
                      " $" + std::to_string(app.loanAmount) +
                      " for " + app.applicant.getName());
    }
}