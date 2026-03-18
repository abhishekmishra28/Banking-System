#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <chrono>

struct TransactionRecord {
    std::string accountId;
    double      amount;
    std::string type;       // "DEPOSIT" | "WITHDRAWAL" | "TRANSFER"
    std::chrono::system_clock::time_point timestamp;
};

class FraudDetectionService {
private:
    double singleTransactionThreshold = 10000.0;
    int    rapidTransactionLimit      = 5;
    int    rapidTransactionWindowSecs = 60;

    std::vector<TransactionRecord> history;

    bool isRapidTransaction(const std::string& accountId) const {
        auto now = std::chrono::system_clock::now();
        int recentCount = 0;
        for (const auto& rec : history) {
            if (rec.accountId == accountId) {
                auto diffSecs = std::chrono::duration_cast<std::chrono::seconds>(
                    now - rec.timestamp).count();
                if (diffSecs <= rapidTransactionWindowSecs) ++recentCount;
            }
        }
        return recentCount >= rapidTransactionLimit;
    }

public:
    bool analyse(const TransactionRecord& txn) {
        bool flagged = false;

        if (txn.amount >= singleTransactionThreshold) {
            std::cout << "[FRAUD ALERT] Large transaction detected: $"
                      << txn.amount << " on account " << txn.accountId << "\n";
            flagged = true;
        }

        if (isRapidTransaction(txn.accountId)) {
            std::cout << "[FRAUD ALERT] Rapid transactions detected on account "
                      << txn.accountId << "\n";
            flagged = true;
        }

        history.push_back(txn);
        return flagged;
    }
};