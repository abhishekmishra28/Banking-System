#pragma once
#include <vector>
#include <optional>
#include <sqlite3.h>
#include "../models/Customer.hpp"
#include "Database.hpp"

// ─────────────────────────────────────────────────────────────
//  CustomerRepo
//  All methods accept / return the Phase-3 Customer type.
// ─────────────────────────────────────────────────────────────
class CustomerRepo {
    sqlite3* db() const { return Database::get().getHandle(); }

public:
    // INSERT OR IGNORE — safe to call even if customer already exists
    void save(const Customer& c) const {
        const char* sql =
            "INSERT OR IGNORE INTO customers (customer_id, name, email, phone) "
            "VALUES (?, ?, ?, ?);";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text(stmt, 1, c.getCustomerId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, c.getName().c_str(),       -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, c.getEmail().c_str(),      -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, c.getPhone().c_str(),      -1, SQLITE_TRANSIENT);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // Returns std::nullopt if not found
    std::optional<Customer> findById(const std::string& customerId) const {
        const char* sql =
            "SELECT customer_id, name, email, phone "
            "FROM customers WHERE customer_id = ?;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text(stmt, 1, customerId.c_str(), -1, SQLITE_TRANSIENT);

        std::optional<Customer> result;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = Customer(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))
            );
        }
        sqlite3_finalize(stmt);
        return result;
    }

    std::vector<Customer> findAll() const {
        const char* sql =
            "SELECT customer_id, name, email, phone FROM customers;";

        sqlite3_stmt* stmt = Database::get().prepare(sql);

        std::vector<Customer> results;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results.emplace_back(
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))
            );
        }
        sqlite3_finalize(stmt);
        return results;
    }
};