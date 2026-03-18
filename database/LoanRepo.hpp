#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sqlite3.h>
#include "../services/LoanService.hpp"   // reuses Phase-3 LoanApplication struct
#include "Database.hpp"

// ─────────────────────────────────────────────────────────────
//  LoanRepo
//  Reuses the LoanApplication struct defined in LoanService.h
// ─────────────────────────────────────────────────────────────
class LoanRepo {
    sqlite3* db() const { return Database::get().getHandle(); }

public:
    // Save application + decision in one row
    void save(const LoanApplication& app,
              bool approved,
              double interestRate) const {

        const char* sql =
            "INSERT OR IGNORE INTO loans "
            "(application_id, customer_id, amount, term_months, "
            " approved, interest_rate) "
            "VALUES (?, ?, ?, ?, ?, ?);";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text  (stmt, 1, app.applicationId.c_str(),           -1, SQLITE_TRANSIENT);
        sqlite3_bind_text  (stmt, 2, app.applicant.getCustomerId().c_str(),-1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 3, app.requestedAmount);
        sqlite3_bind_int   (stmt, 4, app.termMonths);
        sqlite3_bind_int   (stmt, 5, approved ? 1 : 0);
        sqlite3_bind_double(stmt, 6, interestRate);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Print all loans for a customer
    void printByCustomer(const std::string& customerId) const {
        const char* sql =
            "SELECT application_id, amount, term_months, approved, "
            "       interest_rate, created_at "
            "FROM loans WHERE customer_id = ? ORDER BY created_at DESC;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text(stmt, 1, customerId.c_str(), -1, SQLITE_TRANSIENT);

        std::cout << "\n===== Loan History: " << customerId << " =====\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout
                << "ID: "     << sqlite3_column_text  (stmt, 0)
                << " | $"     << sqlite3_column_double(stmt, 1)
                << " / "      << sqlite3_column_int   (stmt, 2) << " months"
                << " | "      << (sqlite3_column_int(stmt, 3) ? "APPROVED" : "DECLINED")
                << " @ "      << (sqlite3_column_double(stmt, 4) * 100.0) << "%"
                << " | "      << sqlite3_column_text  (stmt, 5)
                << "\n";
        }
        std::cout << "==========================================\n";
        sqlite3_finalize(stmt);
    }
};