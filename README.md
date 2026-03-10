# Banking System (C++ OOP Project)

A modular **Banking System built in C++** to demonstrate real-world usage of **Object-Oriented Programming (OOP)** concepts such as encapsulation, inheritance, polymorphism, abstraction, and composition.

This project is designed as a **learning-focused but production-style architecture**, where different responsibilities are separated into models, services, security modules, and infrastructure utilities. The goal is to show how a large application can be structured using OOP principles rather than writing everything inside a single file.

---

# Overview

This project simulates a simplified banking environment where customers can create accounts, perform transactions, apply for loans, and receive notifications. The system also includes fraud detection and audit logging.

The architecture resembles how real backend systems are structured in financial software.

Main capabilities of the system:

* Create customers
* Open savings or current accounts
* Deposit and withdraw money
* Transfer funds between accounts
* Apply interest to savings accounts
* Detect suspicious transactions
* Send transaction and fraud notifications
* Process loan applications
* Maintain a full audit log of system events

---

# Project Structure

The project follows a layered architecture where each folder has a specific responsibility.

```
Banking-System
│
├── models
│   ├── Customer.h
│   ├── Account.h
│   ├── SavingsAccount.h
│   └── CurrentAccount.h
│
├── services
│   ├── AccountService.h
│   ├── TransactionService.h
│   ├── LoanService.h
│   └── NotificationService.h
│
├── security
│   └── FraudDetectionService.h
│
├── infrastructure
│   └── AuditLogs.h
│
└── main.cpp
```

### Models

These represent the core business entities.

* **Customer** – stores customer details
* **Account** – abstract base class for accounts
* **SavingsAccount** – supports interest
* **CurrentAccount** – supports overdraft

### Services

These classes contain the main business logic.

* **AccountService** – manages account creation and lookup
* **TransactionService** – handles deposits, withdrawals, and transfers
* **LoanService** – processes loan applications
* **NotificationService** – sends alerts to customers

### Security

Contains components responsible for system safety.

* **FraudDetectionService** – monitors transactions and flags suspicious activity

### Infrastructure

Utility components used across the system.

* **AuditLogs** – keeps an immutable record of all system events

---

# OOP Concepts Demonstrated

This project intentionally uses all major object-oriented programming principles.

### Encapsulation

Sensitive data such as account balance and customer information are kept private and accessed through controlled methods.

### Inheritance

SavingsAccount and CurrentAccount inherit common behaviour from the base Account class.

### Polymorphism

TransactionService works with references to the Account base class, allowing it to operate on any type of account.

### Abstraction

Account is defined as an abstract class with pure virtual methods such as deposit() and withdraw().

### Composition

Services such as TransactionService use other services (FraudDetectionService, NotificationService, AuditLogs) as internal components.

---

# Features

### Account Management

The system supports two types of accounts:

Savings Account

* Earns interest
* Does not allow overdraft

Current Account

* Allows overdraft up to a defined limit

### Transactions

Users can perform:

* Deposits
* Withdrawals
* Transfers between accounts

Every transaction is recorded and analysed for fraud detection.

### Fraud Detection

Transactions are analysed using simple rules such as:

* Very large transactions
* Rapid repeated transactions

If suspicious activity is detected, the system generates alerts.

### Notifications

Customers receive notifications for:

* Deposits
* Withdrawals
* Transfers
* Fraud alerts
* Loan decisions

Currently notifications are printed to the console but can easily be replaced with email or SMS integrations.

### Loan Processing

LoanService evaluates loan applications based on account balance and defined eligibility rules.

### Audit Logging

Every important system event is recorded in an audit log with a timestamp.
This simulates the audit trails required in real financial systems.

---

# How to Compile and Run

From the root project directory:

```
g++ -std=c++20 -Wall -Wextra -o bank_system main.cpp
```

Run the program:

```
./bank_system
```

On Windows:

```
bank_system.exe
```

---

# Example Flow

When the program runs, it demonstrates the following workflow:

1. Customers are created
2. Accounts are opened
3. Deposits and withdrawals are performed
4. Funds are transferred between accounts
5. Interest is applied to savings accounts
6. Fraud detection is triggered on a large transaction
7. A loan application is processed
8. Final balances are displayed
9. Audit logs are printed

---

# Why This Project Was Built

This project was created to practice and demonstrate:

* Writing clean and modular C++ code
* Applying object-oriented design to a real-world problem
* Structuring medium-sized projects logically
* Designing service-oriented architecture in C++

Instead of focusing only on algorithms, the aim is to understand **how real software systems are designed and organised**.

---

# Possible Improvements

Future improvements could include:

* Database integration (SQLite/PostgreSQL)
* REST API layer
* ATM interface simulation
* Multi-threaded transaction handling
* Authentication and authorization
* Unit testing with Google Test
* Logging frameworks
* Integration with real notification services

---

# Author

Abhishek Kumar Mishra

This project was built as part of learning **Object-Oriented Programming in C++ and system design through practical implementation**.
