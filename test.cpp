#include <iostream>
#include <memory>

#include "models/Customer.hpp"
#include "models/SavingsAccount.hpp"
#include "models/CurrentAccount.hpp"
#include "services/AccountService.hpp"
#include "services/TransactionService.hpp"
#include "services/LoanService.hpp"
#include "services/NotificationService.hpp"
#include "security/FraudDetectionService.hpp"
#include "infrastructure/AuditLogs.hpp"

int main() {
    std::cout << "============================================\n";
    std::cout << "       C++ Banking System — Demo            \n";
    std::cout << "============================================\n\n";

    // ── Infrastructure ──────────────────────────────────────────
    AuditLogs            auditLogs;

    // ── Services ────────────────────────────────────────────────
    NotificationService  notifService;
    FraudDetectionService fraudService;
    TransactionService   txnService(fraudService, notifService, auditLogs);
    AccountService       accountService(auditLogs);
    LoanService          loanService(notifService, auditLogs);

    // ── Customers ───────────────────────────────────────────────
    Customer alice("C001", "Alice Johnson", "alice@bank.com", "+1-555-0101");
    Customer bob  ("C002", "Bob Smith",     "bob@bank.com",   "+1-555-0202");

    std::cout << "\n--- Creating Customers ---\n";
    std::cout << alice.toString() << "\n";
    std::cout << bob.toString()   << "\n";

    // ── Open Accounts ────────────────────────────────────────────
    std::cout << "\n--- Opening Accounts ---\n";
    auto aliceSavings  = accountService.openSavingsAccount(alice, 5000.00);
    auto aliceCurrent  = accountService.openCurrentAccount(alice, 2000.00, 1500.00);
    auto bobSavings    = accountService.openSavingsAccount(bob,   3000.00);

    accountService.listAll();

    // ── Deposits ─────────────────────────────────────────────────
    std::cout << "\n--- Deposits ---\n";
    txnService.deposit(*aliceSavings, 1200.00);
    txnService.deposit(*bobSavings,   800.00);

    // ── Withdrawal ───────────────────────────────────────────────
    std::cout << "\n--- Withdrawal ---\n";
    txnService.withdraw(*aliceSavings, 300.00);

    // ── Transfer ─────────────────────────────────────────────────
    std::cout << "\n--- Transfer Alice → Bob ---\n";
    txnService.transfer(*aliceSavings, *bobSavings, 500.00);

    // ── Apply Savings Interest ────────────────────────────────────
    std::cout << "\n--- Applying Interest ---\n";
    aliceSavings->applyInterest();

    // ── Large Transaction (Fraud Trigger) ─────────────────────────
    std::cout << "\n--- Large Deposit (Fraud Detection Test) ---\n";
    txnService.deposit(*aliceCurrent, 15000.00);

    // ── Loan Processing ───────────────────────────────────────────
    LoanApplication loanApp{
        "LOAN-2024-001",
        alice,
        10000.00,
        36
    };
    loanService.processLoan(loanApp, *aliceSavings);

    // ── Overdraft Test ────────────────────────────────────────────
    std::cout << "\n--- Overdraft Test (CurrentAccount) ---\n";
    try {
        txnService.withdraw(*aliceCurrent, 20000.00);   // Should throw
    } catch (const std::runtime_error& e) {
        std::cout << "[CAUGHT] " << e.what() << "\n";
    }

    // ── Final Balances ────────────────────────────────────────────
    std::cout << "\n--- Final Account Balances ---\n";
    accountService.listAll();

    // ── Audit Log ─────────────────────────────────────────────────
    auditLogs.printAll();

    return 0;
}