#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>

class AuditLogs {
private:
    struct LogEntry {
        std::string timestamp;
        std::string event;
    };

    std::vector<LogEntry> logs;

    static std::string currentTimestamp() {
        auto now  = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::string ts = std::ctime(&t);
        if (!ts.empty() && ts.back() == '\n') ts.pop_back();
        return ts;
    }

public:
    void log(const std::string& event) {
        logs.push_back({ currentTimestamp(), event });
    }

    void printAll() const {
        std::cout << "\n===== AUDIT LOG =====\n";
        for (const auto& entry : logs)
            std::cout << "[" << entry.timestamp << "] " << entry.event << "\n";
        std::cout << "=====================\n";
    }

    size_t count() const { return logs.size(); }
};