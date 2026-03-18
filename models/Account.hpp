#pragma once
#include <string>
#include <stdexcept>
#include "Customer.hpp"

// Abstract base class — cannot be instantiated directly
class Account {
protected:
    std::string accountId;
    double      balance;
    Customer    owner;
    std::string accountType;

public:
    Account(const std::string& id, double initialBalance, const Customer& customer)
        : accountId(id), balance(initialBalance), owner(customer) {}

    virtual ~Account() = default;

    // Pure virtual — every account type must implement these
    virtual void deposit(double amount)  = 0;
    virtual void withdraw(double amount) = 0;
    virtual std::string getAccountType() const = 0;

    // Concrete shared behaviour
    double      getBalance()   const { return balance; }
    std::string getAccountId() const { return accountId; }
    Customer    getOwner()     const { return owner; }

    virtual void transfer(double amount, Account& target) {
        withdraw(amount);
        target.deposit(amount);
    }

    virtual std::string toString() const {
        return "[" + getAccountType() + " " + accountId + "] " +
               owner.getName() + " | Balance: $" + std::to_string(balance);
    }
};