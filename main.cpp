#include <iostream>
#include <memory>
#include <unordered_map>
// ── Phase 3 includes (unchanged) ────────────────────────────
#include "models/Customer.hpp"
#include "models/SavingsAccount.hpp"
#include "models/CurrentAccount.hpp"
#include "services/AccountService.hpp"
#include "services/TransactionService.hpp"
#include "services/LoanService.hpp"
#include "services/NotificationService.hpp"
#include "security/FraudDetectionService.hpp"

// ── Phase 4 additions ────────────────────────────────────────
#include "database/Database.hpp"
#include "database/CustomerRepo.hpp"
#include "database/AccountRepo.hpp"
#include "database/TransactionRepo.hpp"
#include "database/LoanRepo.hpp"
#include "infrastructure/PersistentAuditLogs.hpp"

// ── Helper ───────────────────────────────────────────────────
static void section(const std::string& title) {
    std::cout << "\n" << std::string(50, '-') << "\n"
              << "  " << title << "\n"
              << std::string(50, '-') << "\n";
}

int main() {
    std::cout << "================================================\n";
    std::cout << "  C++ Banking System \n";
    std::cout << "================================================\n\n";

    // ── 1. Open the database (creates bank.db if absent) ─────────
    section("1. Database Initialisation");
    Database::get().open("bank.db");

    // ── 2. Repositories ──────────────────────────────────────────
    CustomerRepo    customerRepo;
    AccountRepo     accountRepo;
    TransactionRepo txnRepo;
    LoanRepo        loanRepo;

    // ── 3. Infrastructure — swap in PersistentAuditLogs ──────────
    //    All services receive AuditLogs& — Liskov holds perfectly.
    PersistentAuditLogs auditLogs;

    // ── 4. Services — IDENTICAL constructors to main.cpp ─────────
    NotificationService   notifService;
    FraudDetectionService fraudService;
    TransactionService    txnService(fraudService, notifService, auditLogs);
    AccountService        accountService(auditLogs);
    LoanService           loanService(notifService, auditLogs);

    // ── 5. Customers ──────────────────────────────────────────────
    section("2. Create & Persist Customers");
    Customer alice("C001", "Alice Johnson", "alice@bank.com", "+1-555-0101");
    Customer bob  ("C002", "Bob Smith",     "bob@bank.com",   "+1-555-0202");

    customerRepo.save(alice);   // Phase 4: persist
    customerRepo.save(bob);

    std::cout << alice.toString() << "\n";
    std::cout << bob.toString()   << "\n";

    // ── 6. Open accounts ─────────────────────────────────────────
    section("3. Open & Persist Accounts");
    auto aliceSavings = accountService.openSavingsAccount(alice, 5000.00);
    auto aliceCurrent = accountService.openCurrentAccount(alice, 2000.00, 1500.00);
    auto bobSavings   = accountService.openSavingsAccount(bob,   3000.00);

    accountRepo.save(*aliceSavings);   // Phase 4: persist each account
    accountRepo.save(*aliceCurrent);
    accountRepo.save(*bobSavings);

    accountService.listAll();

    // ─────────────────────────────────────────────────────────────
    //  Helper lambda: wraps txnService.deposit() and persists
    //  both the updated balance and the transaction record.
    //  The lambda captures by reference — no new classes needed.
    // ─────────────────────────────────────────────────────────────
    auto persistDeposit = [&](Account& acc, double amount) {
        txnService.deposit(acc, amount);                            // Phase 3

        accountRepo.updateBalance(acc);                             // Phase 4
        TransactionRecord rec{ acc.getAccountId(), amount,
                               "DEPOSIT",
                               std::chrono::system_clock::now() };
        txnRepo.save(rec, false);
    };

    auto persistWithdraw = [&](Account& acc, double amount) {
        txnService.withdraw(acc, amount);

        accountRepo.updateBalance(acc);
        TransactionRecord rec{ acc.getAccountId(), amount,
                               "WITHDRAWAL",
                               std::chrono::system_clock::now() };
        txnRepo.save(rec, false);
    };

    auto persistTransfer = [&](Account& src, Account& tgt, double amount) {
        txnService.transfer(src, tgt, amount);

        accountRepo.updateBalance(src);
        accountRepo.updateBalance(tgt);
        TransactionRecord rec{ src.getAccountId(), amount,
                               "TRANSFER",
                               std::chrono::system_clock::now() };
        txnRepo.save(rec, false, tgt.getAccountId());
    };

    // ── 7. Deposits ───────────────────────────────────────────────
    section("4. Deposits");
    persistDeposit(*aliceSavings, 1200.00);
    persistDeposit(*bobSavings,   800.00);

    // ── 8. Withdrawal ─────────────────────────────────────────────
    section("5. Withdrawal");
    persistWithdraw(*aliceSavings, 300.00);

    // ── 9. Transfer ───────────────────────────────────────────────
    section("6. Transfer Alice -> Bob");
    persistTransfer(*aliceSavings, *bobSavings, 500.00);

    // ── 10. Apply Interest ────────────────────────────────────────
    section("7. Apply Savings Interest");
    aliceSavings->applyInterest();
    accountRepo.updateBalance(*aliceSavings);   // keep DB in sync

    // ── 11. Large deposit — fraud trigger ─────────────────────────
    section("8. Large Deposit (Fraud Detection Test)");
    txnService.deposit(*aliceCurrent, 15000.00);
    accountRepo.updateBalance(*aliceCurrent);
    {
        TransactionRecord rec{ aliceCurrent->getAccountId(), 15000.00,
                               "DEPOSIT",
                               std::chrono::system_clock::now() };
        txnRepo.save(rec, /*flagged=*/true);   // mark as flagged in DB
    }

    // ── 12. Loan ──────────────────────────────────────────────────
    section("9. Loan Processing");
    LoanApplication loanApp{ "LOAN-2024-001", alice, 10000.00, 36 };
    loanService.processLoan(loanApp, *aliceSavings);
    accountRepo.updateBalance(*aliceSavings);           // loan disbursed
    loanRepo.save(loanApp, /*approved=*/true, 0.07);

    // ── 13. Overdraft test ────────────────────────────────────────
    section("10. Overdraft Test");
    try {
        txnService.withdraw(*aliceCurrent, 20000.00);
    } catch (const std::runtime_error& e) {
        std::cout << "[CAUGHT] " << e.what() << "\n";
    }

    // ── 14. Reload from database ──────────────────────────────────
    section("11. Reload All Data from Database");
    auto dbCustomers = customerRepo.findAll();
    auto dbAccounts  = accountRepo.findAll(dbCustomers);

    std::cout << "\nCustomers in DB:\n";
    for (const auto& c : dbCustomers)
        std::cout << "  " << c.toString() << "\n";

    std::cout << "\nAccounts in DB:\n";
    for (const auto& a : dbAccounts)
        std::cout << "  " << a->toString() << "\n";

    // ── 15. Transaction histories ─────────────────────────────────
    section("12. Transaction Histories");
    txnRepo.printHistory(aliceSavings->getAccountId());
    txnRepo.printHistory(bobSavings->getAccountId());

    std::cout << "\nTotal flagged transactions: "
              << txnRepo.countFlagged() << "\n";

    // ── 16. Loan history ──────────────────────────────────────────
    section("13. Loan History");
    loanRepo.printByCustomer(alice.getCustomerId());

    // ── 17. Audit logs ────────────────────────────────────────────
    section("14. Audit Logs");
    auditLogs.printAll();          // in-memory (Phase 3 method — unchanged)
    auditLogs.printFromDb();       // persistent (Phase 4 method)

    // ── 18. Close database ────────────────────────────────────────
    Database::get().close();

    std::cout << "\n✔  Done. Inspect with:  sqlite3 bank.db\n";
    return 0;
}