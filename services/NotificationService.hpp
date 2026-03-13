#pragma once
#include <string>
#include <iostream>
#include "../models/Account.hpp"

class NotificationService {
public:
    void sendTransactionAlert(
        const Account& acc, 
        const std::string& type,
        double amt
    ){
        std::cout<<"[Notification] Email to "<<acc.getOwner().getEmail() << " ---  " << type << "of Rs." << amt <<" on account "<< acc.getAccountId() << " | Current Baance : Rs." << acc.getBalance() << std::endl;
    }

    void sendFraudAlert(const Account& acc){
        std::cout<<"[Notification : Fraud Alert] SMS to "
        << acc.getOwner().getPhone()
        << " -- Suspecious Activity Detected on Your Account "
        << acc.getAccountId() << ". Please Verify.\n";
    }

    void sendLoanDecision(
        const Customer& customer,
        double amt,
        bool approved,
        double rate = 0.0
    ){
        if(approved){
            std::cout<< "[Loan Notification] Email to : "<<customer.getEmail()
            <<" -- Loan of Rs."<<amt<<" has been approved at the rate of "<< (rate*100) << "% intrest.\n";
        }else{
            std::cout<<"[Loan Notification] Email to : "<<customer.getEmail()
            <<" -- Loan of Rs."<<amt<<" is Declined.\n";
        }
    }
};