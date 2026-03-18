#pragma once
#include "Account.hpp"
#include <iostream>

class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(const std::string& id,
                   double initialBalance,
                   const Customer& customer,
                   double overdraft = 1000.0)
        : Account(id, initialBalance, customer), overdraftLimit(overdraft) {}

    std::string getAccountType() const override { return "CurrentAccount"; }

    void deposit(double amount) override {
        if (amount <= 0) throw std::invalid_argument("Deposit must be positive");
        balance += amount;
        std::cout << "[Current] Deposited $" << amount
                  << " | New balance: $" << balance << "\n";
    }

    void withdraw(double amount) override {
        if (amount <= 0) throw std::invalid_argument("Amount must be positive");
        if (amount > balance + overdraftLimit)
            throw std::runtime_error("Overdraft limit exceeded");
        balance -= amount;
        std::cout << "[Current] Withdrew $" << amount
                  << " | New balance: $" << balance << "\n";
    }

    double getOverdraftLimit() const { return overdraftLimit; }
};