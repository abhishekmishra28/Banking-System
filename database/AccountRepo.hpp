#pragma once
#include <vector>
#include <memory>
#include <sqlite3.h>
#include "../models/Account.hpp"
#include "../models/SavingsAccount.hpp"
#include "../models/CurrentAccount.hpp"
#include "Database.hpp"

// ─────────────────────────────────────────────────────────────
//  AccountRepo
//  Saves any Account* subclass; rehydrates into the correct
//  concrete type on load — polymorphism meets persistence.
// ─────────────────────────────────────────────────────────────
class AccountRepo {
    sqlite3* db() const { return Database::get().getHandle(); }

public:
    // INSERT OR IGNORE — detects subtype via dynamic_cast
    void save(const Account& acc) const {
        const char* sql =
            "INSERT OR IGNORE INTO accounts "
            "(account_id, account_type, balance, customer_id, "
            " interest_rate, overdraft_limit) "
            "VALUES (?, ?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt = Database::get().prepare(sql);

        // Type-specific fields
        double interestRate   = -1.0;   // sentinel = NULL
        double overdraftLimit = -1.0;

        if (const auto* sav = dynamic_cast<const SavingsAccount*>(&acc))
            interestRate = sav->getInterestRate();
        else if (const auto* cur = dynamic_cast<const CurrentAccount*>(&acc))
            overdraftLimit = cur->getOverdraftLimit();

        sqlite3_bind_text  (stmt, 1, acc.getAccountId().c_str(),             -1, SQLITE_TRANSIENT);
        sqlite3_bind_text  (stmt, 2, acc.getAccountType().c_str(),           -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, acc.getBalance());
        sqlite3_bind_text  (stmt, 4, acc.getOwner().getCustomerId().c_str(), -1, SQLITE_TRANSIENT);

        // Bind NULL when the field does not apply to this subtype
        if (interestRate   >= 0) sqlite3_bind_double(stmt, 5, interestRate);
        else                     sqlite3_bind_null  (stmt, 5);
        if (overdraftLimit >= 0) sqlite3_bind_double(stmt, 6, overdraftLimit);
        else                     sqlite3_bind_null  (stmt, 6);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // UPDATE balance after every transaction — keeps DB in sync with in-memory object
    void updateBalance(const Account& acc) const {
        const char* sql =
            "UPDATE accounts SET balance = ? WHERE account_id = ?;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_double(stmt, 1, acc.getBalance());
        sqlite3_bind_text  (stmt, 2, acc.getAccountId().c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Load all accounts, rehydrating each into the correct concrete subclass.
    // Requires a customer lookup to reconstruct the Customer value object.
    std::vector<std::shared_ptr<Account>>
    findAll(const std::vector<Customer>& customers) const {
        const char* sql =
            "SELECT account_id, account_type, balance, customer_id, "
            "       interest_rate, overdraft_limit "
            "FROM accounts;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);

        // Build a quick lookup map from the supplied customer list
        std::unordered_map<std::string, Customer> custMap;
        for (const auto& c : customers) custMap[c.getCustomerId()] = c;

        std::vector<std::shared_ptr<Account>> results;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string accountId   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string accountType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double      balance     = sqlite3_column_double(stmt, 2);
            std::string customerId  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

            // Resolve the owner
            Customer owner;
            auto it = custMap.find(customerId);
            if (it != custMap.end()) owner = it->second;

            std::shared_ptr<Account> acc;

            if (accountType == "SavingsAccount") {
                double rate = (sqlite3_column_type(stmt, 4) != SQLITE_NULL)
                              ? sqlite3_column_double(stmt, 4)
                              : 0.035;
                acc = std::make_shared<SavingsAccount>(accountId, balance, owner, rate);

            } else if (accountType == "CurrentAccount") {
                double limit = (sqlite3_column_type(stmt, 5) != SQLITE_NULL)
                               ? sqlite3_column_double(stmt, 5)
                               : 1000.0;
                acc = std::make_shared<CurrentAccount>(accountId, balance, owner, limit);
            }

            if (acc) results.push_back(std::move(acc));
        }

        sqlite3_finalize(stmt);
        return results;
    }
};