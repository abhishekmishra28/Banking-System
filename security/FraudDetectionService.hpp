#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <vector>

struct TransactionRecord {
    std::string accountId;
    double amount;
    std::string type; // Deposit / Writhdraw / Transfer
    std::chrono::system_clock::time_point timestamp;
};

class FraudDetectionService {
private:
    double  SingleTransLimit   =   10000.0;
    int     FrequencyLimit     =   5;
    int     TransactionWinSec  =   60;

    std::vector<TransactionRecord> history;

    bool isRapidTransaction(const std::string &accountId) const {
        auto now = std::chrono::system_clock::now();
        int recentCount = 0;
        for(const auto& rec : history) {
            if(rec.accountId == accountId) {
                auto diffSecs = std::chrono::duration_cast<std::chrono::seconds>(now - rec.timestamp).count();
                if(diffSecs <= TransactionWinSec) recentCount++;
            }
        }
        return recentCount >= FrequencyLimit;
    }

public:
    bool analyze(const TransactionRecord& txn){
        bool flagged = false;
        if(txn.amount >= SingleTransLimit){
            std::cout<<"[Fraud Alert] Large Transaction Detected : Rs."<< txn.amount << " on account " <<txn.accountId<<std::endl;
            flagged = true;
        }
        if(isRapidTransaction(txn.accountId)) {
            std::cout<<"[Fraud Alert] Large Transaction Detected on account " << txn.accountId<<std::endl;
            flagged = true;
        }
        history.push_back(txn);
        return flagged;
    }
};