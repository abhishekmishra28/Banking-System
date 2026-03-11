#pragma once
#include <iostream>
#include <string>
#include "Account.hpp"
#include <stdexcept>

class currentAccount : public Account {
private:
    double overDraftLimit;
public:
    currentAccount(
        const std::string& id,
        double initialBalance,
        Customer& customer,
        double overDraft = 1000.0
    ) : Account(accountId,initialBalance,customer), overDraftLimit(overDraft) {}

    // Account Type
    std::string getAccountType() const {return "CurrentAccount";}

    // Deposit Method
    void deposit(double amount) override {
        if(amount <=0 ) throw std::invalid_argument("Amount must be positive.");
        balance += amount;
        std::cout<<"[Current] Deposited Rs. "<<amount<<" | Current Balance : Rs."<<balance<<std::endl;
    }

    // Withdraw Method
    void withdraw(double amount) override {
        if(amount <=0 ) throw std::invalid_argument("Amount must be positive.");
        if(amount > balance + overDraftLimit) throw std::runtime_error("OverDraftLimit Exceed : Insufficient Funds.");
        balance-=amount;
        std::cout<<"[Current] withdrew Rs. "<<amount<<" | Current Balance : Rs."<<balance<<std::endl;
    }

    double getOverDraftLimit() const {return overDraftLimit;}
};