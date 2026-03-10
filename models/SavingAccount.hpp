#pragma once
#include <iostream>
#include <string>
#include "Account.hpp"

class savingAccount : public Account {
private:
    double interestRate;
public:
    savingAccount(const std::string& id,
                double initialBalance,
                const Customer& customer,
                double rate = 0.035)
        : Account(id,initialBalance,customer), interestRate(rate) {}
    
    // Account Type
    std::string getAccountType() const override {return "SavingsAccount";}

    double getInterest() const {return interestRate;}
    // Define Account Class virtual methods
    void deposit(double amount) override {
        if(amount <= 0) throw std::invalid_argument("Deposit Amount must be Positive.");
        balance += amount;
        std::cout<<"[Savings] Deposited : Rs." << amount <<". | Current Balance : Rs."<<balance<<std::endl;
    }
    void withdraw(double amount) override {
        if(amount <= 0) throw std::invalid_argument("Amount must be Positive.");
        if(amount>balance) throw std::runtime_error("Insufficient Funds.");
        balance-=amount;
        std::cout<<"[Savings] Withdrawn : Rs."<<amount<<". | Current Balance : Rs."<<balance<<std::endl;
    }

    void applyIntrest() {
        double interest = balance * interestRate;
        balance += interest;
        std::cout<<"[Savings] Intrest Applied : Rs."<<interest<<". | Current Balance : Rs."<<balance<<std::endl;
    }

};