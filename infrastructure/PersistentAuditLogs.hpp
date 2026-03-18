#pragma once
#include "../infrastructure/AuditLogs.hpp"
#include "../database/Database.hpp"
#include <sqlite3.h>
#include <iostream>

// ─────────────────────────────────────────────────────────────
//  PersistentAuditLogs
//
//  Extends AuditLogs (Phase 3) via inheritance.
//  Overrides log() to ALSO write every entry to the database.
//  The parent's in-memory log and printAll() are fully preserved.
//
//  Usage: pass a PersistentAuditLogs wherever AuditLogs& is expected.
//  No other code changes needed.
// ─────────────────────────────────────────────────────────────
class PersistentAuditLogs : public AuditLogs {
public:
    // Shadows AuditLogs::log() — writes to DB then to memory
    void log(const std::string& event) {
        // 1. Persist to database
        const char* sql =
            "INSERT INTO audit_log (event) VALUES (?);";

        sqlite3_stmt* stmt = Database::get().prepare(sql);
        sqlite3_bind_text(stmt, 1, event.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        // 2. Delegate to parent for in-memory storage and timestamp
        AuditLogs::log(event);
    }

    // Query the persistent log from the database (survives restarts)
    void printFromDb(int limit = 50) const {
        std::string sql =
            "SELECT event, created_at FROM audit_log "
            "ORDER BY id DESC LIMIT " + std::to_string(limit) + ";";

        sqlite3_stmt* stmt = Database::get().prepare(sql);

        std::cout << "\n===== PERSISTENT AUDIT LOG (DB, last "
                  << limit << ") =====\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::cout << "[" << sqlite3_column_text(stmt, 1) << "] "
                      << sqlite3_column_text(stmt, 0) << "\n";
        }
        std::cout << "====================================================\n";
        sqlite3_finalize(stmt);
    }
};