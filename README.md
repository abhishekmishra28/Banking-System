# OOP Banking System in C++

A beginner-to-advanced guide covering every OOP pillar in C++ through a fully modular Banking System. Built with C++20, with an optional SQLite persistence layer added in Phase 4 — without changing a single line of existing code.

---

## What This Project Covers

- **Phase 1** — OOP fundamentals: classes, encapsulation, inheritance, polymorphism, abstraction, and composition
- **Phase 2** — System design: how to translate OOP concepts into a real modular architecture
- **Phase 3** — Full implementation: a working banking system with accounts, transactions, fraud detection, loans, notifications, and audit logs
- **Phase 4** — Database integration: a complete SQLite persistence layer added purely additively

---

## Features

- Savings and Current account types with polymorphic behaviour
- Deposit, withdrawal, and transfer operations
- Fraud detection (large transactions, rapid activity)
- Loan processing with credit scoring
- Email/SMS notification system
- Immutable audit trail
- SQLite persistence via the Repository pattern

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────┐
│                        C++ Banking System                           │
│                                                                     │
│  ┌─────────────────────┐   ┌─────────────────────────────────────┐  │
│  │       models/       │   │             services/               │  │
│  │                     │   │                                     │  │
│  │  Customer           │   │  AccountService                     │  │
│  │  Account  (abstract)│   │  TransactionService                 │  │
│  │  SavingsAccount     │   │  LoanService                        │  │
│  │  CurrentAccount     │   │  NotificationService                │  │
│  └─────────────────────┘   └─────────────────────────────────────┘  │
│                                                                     │
│  ┌─────────────────────┐   ┌─────────────────────────────────────┐  │
│  │      security/      │   │          infrastructure/            │  │
│  │                     │   │                                     │  │
│  │  FraudDetection     │   │  AuditLogs                          │  │
│  │  Service            │   │  PersistentAuditLogs  (Phase 4)     │  │
│  └─────────────────────┘   └─────────────────────────────────────┘  │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐    │
│  │                    database/  (Phase 4)                     │    │
│  │                                                             │    │
│  │   Database · CustomerRepo · AccountRepo                     │    │
│  │   TransactionRepo · LoanRepo                                │    │
│  └─────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Project Structure

```
bank-system/
├── models/
│   ├── Customer.h
│   ├── Account.h              ← abstract base class
│   ├── SavingsAccount.h
│   └── CurrentAccount.h
├── services/
│   ├── AccountService.h
│   ├── TransactionService.h
│   ├── LoanService.h
│   └── NotificationService.h
├── security/
│   └── FraudDetectionService.h
├── infrastructure/
│   ├── AuditLogs.h
│   └── PersistentAuditLogs.h  ← Phase 4
├── database/                  ← Phase 4
│   ├── Database.h
│   ├── CustomerRepo.h
│   ├── AccountRepo.h
│   ├── TransactionRepo.h
│   └── LoanRepo.h
├── test.cpp                   ← Phase 3 entry point
└── main.cpp                   ← Phase 4 entry point
```

---

## OOP Concept Map

```
                     ┌──────────────────────────────┐
                     │         <<abstract>>         │
                     │           Account            │
                     │                              │
                     │  # accountId : string        │
                     │  # balance   : double        │
                     │  # owner     : Customer      │
                     │                              │
                     │  + deposit()   = 0           │
                     │  + withdraw()  = 0           │
                     │  + getType()   = 0           │
                     │  + transfer()  ← concrete    │
                     └───────────────┬──────────────┘
                                     │
                      ───────────────────────────────
                      │                             │
          ┌───────────▼────────────┐   ┌────────────▼──────────────┐
          │     SavingsAccount     │   │      CurrentAccount       │
          │                        │   │                           │
          │  - interestRate        │   │  - overdraftLimit         │
          │  + applyInterest()     │   │  allows negative balance  │
          └────────────────────────┘   └───────────────────────────┘


   ┌──────────────┐
   │   Customer   │ ◄──── composed into ──── Account   (has-a)
   │              │
   │  - id        │
   │  - name      │
   │  - email     │
   │  - phone     │
   └──────────────┘


   ┌───────────────────────┐
   │   TransactionService  │ ──► FraudDetectionService
   │   (pipeline coord.)   │ ──► NotificationService
   │                       │ ──► AuditLogs
   └───────────────────────┘
             │
             │  accepts Account& — runtime polymorphic dispatch
             ▼
      deposit() / withdraw() / transfer()
      correct method called based on actual subtype


   ┌─────────────────────┐
   │      AuditLogs      │                    Phase 3
   │  + log(event)       │
   │  + printAll()       │
   └──────────┬──────────┘
              │  inherits
   ┌──────────▼──────────┐
   │  PersistentAuditLogs│                    Phase 4
   │  + log()  → DB + ↑  │  passes silently as AuditLogs& anywhere
   │  + printFromDb()    │  Liskov Substitution Principle
   └─────────────────────┘
```

---

## Transaction Pipeline

```
  txnService.deposit(account, amount)
       │
       ├─► account.deposit(amount)          ← polymorphic dispatch
       │
       ├─► fraudService.analyse(txn)        ← heuristic rules
       │        │
       │        ├── amount >= $10,000  → flag
       │        ├── > 5 txns / 60 sec  → flag
       │        └── midnight – 4 am    → flag
       │
       ├─► notifService.sendAlert(...)      ← email notification
       │       └── if flagged → sendFraudAlert()   ← SMS alert
       │
       └─► auditLogs.log(...)              ← immutable record
```

---

## Database Schema

```
┌───────────────────────────────────────────┐
│                 customers                 │
│ ───────────────────────────────────────   │
│  customer_id   TEXT   PRIMARY KEY         │
│  name          TEXT   NOT NULL            │
│  email         TEXT   NOT NULL  UNIQUE    │
│  phone         TEXT   NOT NULL            │
└────────────────────┬──────────────────────┘
                     │ 1
                     │
                     │ N
┌──────────────────────────────────────────┐
│                  accounts                │
│ ───────────────────────────────────────  │
│  account_id      TEXT   PRIMARY KEY      │
│  account_type    TEXT   NOT NULL         │
│    'SavingsAccount' | 'CurrentAccount'   │
│  balance         REAL   NOT NULL         │
│  customer_id     TEXT   FK → customers   │
│  interest_rate   REAL   NULL for Current │
│  overdraft_limit REAL   NULL for Savings │
└──────┬──────────────────────┬────────────┘
       │ 1                    │ 1
       │                      │
       │ N                    │ N
┌──────▼──────────────┐  ┌───▼───────────────────────┐
│     transactions    │  │          loans            │
│  ─────────────────  │  │  ─────────────────────────│
│  id       INT  PK   │  │  application_id TEXT  PK  │
│  account_id    FK   │  │  customer_id         FK   │
│  type      TEXT     │  │  amount         REAL      │
│    DEPOSIT          │  │  term_months    INT       │
│    WITHDRAWAL       │  │  approved       INT  0/1  │
│    TRANSFER         │  │  interest_rate  REAL      │
│  amount    REAL     │  │  created_at     TEXT      │
│  target_account     │  └───────────────────────────┘
│  flagged   INT 0/1  │
│  created_at TEXT    │  ┌────────────────────────────┐
└─────────────────────┘  │         audit_log          │
                         │  ───────────────────────── │
                         │  id        INT   PK        │
                         │  event     TEXT  NOT NULL  │
                         │  created_at TEXT           │
                         └────────────────────────────┘
```

---

## Phase 4 — Zero Changes Rule

```
Phase 3 files                    Phase 4 additions
────────────────────────         ─────────────────────────────────
models/        ← untouched       database/Database.h
services/      ← untouched       database/CustomerRepo.h
security/      ← untouched       database/AccountRepo.h
infrastructure/AuditLogs.h       database/TransactionRepo.h
               ← untouched       database/LoanRepo.h
test.cpp       ← untouched       infrastructure/PersistentAuditLogs.h
                                 main.cpp   ← new entry point
```

Adding persistence to an existing codebase means wrapping the domain — not rewriting it. This demonstrates the **Open/Closed Principle**: open for extension, closed for modification.

---

## OOP Concepts Used

| Concept | Where |
|---|---|
| Encapsulation | `Account`, `Customer` — all attributes private, mutated through validated methods |
| Inheritance | `SavingsAccount` and `CurrentAccount` extend `Account`; `PersistentAuditLogs` extends `AuditLogs` |
| Polymorphism | `TransactionService` accepts `Account&` — correct `deposit()`/`withdraw()` called at runtime |
| Abstraction | `Account` is abstract — no bare account can be created; subtypes must fulfil the contract |
| Composition | `Account` has-a `Customer`; `TransactionService` has-a fraud, notification, and audit dependencies |

---

## Getting Started

### Requirements

- A C++20 compiler (`g++` or `clang++`)
- SQLite3 (Phase 4 only)

**Install SQLite3:**

```bash
# Ubuntu / Debian
sudo apt-get install -y libsqlite3-dev

# macOS
brew install sqlite3
```

### Build & Run

**Phase 3 (no database):**
```bash
g++ -std=c++20 -Wall -Wextra -o bank test.cpp
./bank
```

**Phase 4 (with SQLite):**
```bash
g++ -std=c++20 -Wall -Wextra -o bank_db main.cpp -lsqlite3
./bank_db
```

### Inspect the Database

```bash
sqlite3 bank.db
```

```sql
-- Current balances
SELECT account_id, account_type, balance FROM accounts;

-- Full transaction log, newest first
SELECT account_id, type, amount, flagged, created_at
FROM transactions ORDER BY id DESC;

-- Flagged transactions only
SELECT * FROM transactions WHERE flagged = 1;

-- Audit trail
SELECT event, created_at FROM audit_log ORDER BY id DESC LIMIT 20;

.quit
```

---

## License

MIT