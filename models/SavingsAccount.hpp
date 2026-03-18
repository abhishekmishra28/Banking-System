#pragma once
#include "Account.hpp"
#include <iostream>

class SavingsAccount : public Account {
private:
    double interestRate;   // Annual rate, e.g. 0.05 = 5 %

public:
    SavingsAccount(const std::string& id,
                   double initialBalance,
                   const Customer& customer,
                   double rate = 0.035)
        : Account(id, initialBalance, customer), interestRate(rate) {}

    std::string getAccountType() const override { return "SavingsAccount"; }

    void deposit(double amount) override {
        if (amount <= 0) throw std::invalid_argument("Deposit must be positive");
        balance += amount;
        std::cout << "[Savings] Deposited $" << amount
                  << " | New balance: $" << balance << "\n";
    }

    void withdraw(double amount) override {
        if (amount <= 0)      throw std::invalid_argument("Amount must be positive");
        if (amount > balance) throw std::runtime_error("Insufficient funds");
        balance -= amount;
        std::cout << "[Savings] Withdrew $" << amount
                  << " | New balance: $" << balance << "\n";
    }

    void applyInterest() {
        double interest = balance * interestRate;
        balance += interest;
        std::cout << "[Savings] Interest applied: $" << interest
                  << " | New balance: $" << balance << "\n";
    }

    double getInterestRate() const { return interestRate; }
};