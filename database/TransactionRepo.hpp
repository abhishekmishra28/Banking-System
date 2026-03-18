#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sqlite3.h>
#include "../security/FraudDetectionService.hpp"   // reuses Phase-3 TransactionRecord
#include "Database.hpp"

// ─────────────────────────────────────────────────────────────
//  TransactionRepo
//  Reuses the TransactionRecord struct from FraudDetectionService
//  so no new data types are introduced.
// ─────────────────────────────────────────────────────────────
class TransactionRepo {
    sqlite3* db() const { return Database::get().getHandle(); }

public:
    // Save a TransactionRecord (Phase-3 struct) with an optional transfer target
    // and the fraud flag returned by FraudDetectionService::analyse()
    void save(const TransactionRecord& txn,
              bool flagged,
              const std::string& targetAccountId = "") const {

        const char* sql =
            "INSERT INTO transactions "
            "(account_id, type, amount, target_account, flagged) "
            "VALUES (?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text   (stmt, 1, txn.accountId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text   (stmt, 2, txn.type.c_str(),      -1, SQLITE_TRANSIENT);
        sqlite3_bind_double (stmt, 3, txn.amount);

        if (targetAccountId.empty()) sqlite3_bind_null(stmt, 4);
        else sqlite3_bind_text(stmt, 4, targetAccountId.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_bind_int(stmt, 5, flagged ? 1 : 0);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Retrieve full history for one account, newest first
    void printHistory(const std::string& accountId) const {
        const char* sql =
            "SELECT type, amount, target_account, flagged, created_at "
            "FROM transactions "
            "WHERE account_id = ? "
            "ORDER BY id DESC;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text(stmt, 1, accountId.c_str(), -1, SQLITE_TRANSIENT);

        std::cout << "\n===== Transaction History: " << accountId << " =====\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string type      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            double      amount    = sqlite3_column_double(stmt, 1);
            bool        flagged   = sqlite3_column_int(stmt, 3) == 1;
            std::string createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            std::string target    = (sqlite3_column_type(stmt, 2) != SQLITE_NULL)
                                    ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))
                                    : "";

            std::cout << "[" << createdAt << "] "
                      << type << " $" << amount;
            if (!target.empty()) std::cout << " -> " << target;
            if (flagged)         std::cout << " FLAGGED";
            std::cout << "\n";
        }
        std::cout << "=============================================\n";
        sqlite3_finalize(stmt);
    }

    // Count flagged transactions — useful for a compliance report
    int countFlagged() const {
        const char* sql =
            "SELECT COUNT(*) FROM transactions WHERE flagged = 1;";
        sqlite3_stmt* stmt = Database::get().prepare(sql);
        int count = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW)
            count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count;
    }
};