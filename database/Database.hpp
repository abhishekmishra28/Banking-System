#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
#include <sqlite3.h>

// ─────────────────────────────────────────────────────────────
//  Database — Singleton that owns the SQLite connection.
//  All repositories obtain the raw handle via getHandle().
// ─────────────────────────────────────────────────────────────
class Database {
private:
    sqlite3*    handle = nullptr;
    std::string path;

    Database() = default;

    // Execute a statement that returns no rows (CREATE, INSERT, UPDATE, DELETE)
    void exec(const std::string& sql) const {
        char* errMsg = nullptr;
        if (sqlite3_exec(handle, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string msg = errMsg ? errMsg : "unknown";
            sqlite3_free(errMsg);
            throw std::runtime_error("[Database] SQL error: " + msg);
        }
    }

    void createSchema() const {
        exec("PRAGMA foreign_keys = ON;");

        exec(R"(
            CREATE TABLE IF NOT EXISTS customers (
                customer_id TEXT PRIMARY KEY,
                name        TEXT NOT NULL,
                email       TEXT NOT NULL UNIQUE,
                phone       TEXT NOT NULL
            );
        )");

        exec(R"(
            CREATE TABLE IF NOT EXISTS accounts (
                account_id      TEXT PRIMARY KEY,
                account_type    TEXT NOT NULL,
                balance         REAL NOT NULL,
                customer_id     TEXT NOT NULL,
                interest_rate   REAL,
                overdraft_limit REAL,
                FOREIGN KEY (customer_id) REFERENCES customers(customer_id)
            );
        )");

        exec(R"(
            CREATE TABLE IF NOT EXISTS transactions (
                id             INTEGER PRIMARY KEY AUTOINCREMENT,
                account_id     TEXT    NOT NULL,
                type           TEXT    NOT NULL,
                amount         REAL    NOT NULL,
                target_account TEXT,
                flagged        INTEGER NOT NULL DEFAULT 0,
                created_at     TEXT    NOT NULL DEFAULT (datetime('now')),
                FOREIGN KEY (account_id) REFERENCES accounts(account_id)
            );
        )");

        exec(R"(
            CREATE TABLE IF NOT EXISTS loans (
                application_id TEXT    PRIMARY KEY,
                customer_id    TEXT    NOT NULL,
                amount         REAL    NOT NULL,
                term_months    INTEGER NOT NULL,
                approved       INTEGER NOT NULL,
                interest_rate  REAL,
                created_at     TEXT    NOT NULL DEFAULT (datetime('now')),
                FOREIGN KEY (customer_id) REFERENCES customers(customer_id)
            );
        )");

        exec(R"(
            CREATE TABLE IF NOT EXISTS audit_log (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                event      TEXT NOT NULL,
                created_at TEXT NOT NULL DEFAULT (datetime('now'))
            );
        )");
    }

public:
    // ── Singleton access ────────────────────────────────────────
    static Database& get() {
        static Database instance;
        return instance;
    }

    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    // ── Lifecycle ───────────────────────────────────────────────
    void open(const std::string& filePath = "bank.db") {
        path = filePath;
        if (sqlite3_open(filePath.c_str(), &handle) != SQLITE_OK)
            throw std::runtime_error("[Database] Cannot open: " +
                                     std::string(sqlite3_errmsg(handle)));
        std::cout << "[Database] Connected -> " << filePath << "\n";
        createSchema();
        std::cout << "[Database] Schema ready.\n";
    }

    void close() {
        if (handle) {
            sqlite3_close(handle);
            handle = nullptr;
            std::cout << "[Database] Connection closed.\n";
        }
    }

    ~Database() { close(); }

    // ── Raw handle for repositories ─────────────────────────────
    sqlite3* getHandle() const {
        if (!handle) throw std::runtime_error("[Database] Not open.");
        return handle;
    }

    // ── Atomic transaction helpers ───────────────────────────────
    void begin()    const { exec("BEGIN;");    }
    void commit()   const { exec("COMMIT;");   }
    void rollback() const { exec("ROLLBACK;"); }

    // ── Utility: prepare a statement safely ─────────────────────
    sqlite3_stmt* prepare(const std::string& sql) const {
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(handle, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
            throw std::runtime_error("[Database] Prepare failed: " +
                                     std::string(sqlite3_errmsg(handle)));
        return stmt;   // caller must sqlite3_finalize(stmt)
    }
};