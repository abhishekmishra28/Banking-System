#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>
#include <sstream>

class AuditLogs{
private:
    struct LogEntry {
        std::string timestamp;
        std::string event;
    };
    std::vector<LogEntry> logs;

    static std::string currentTimeStamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::string ts = std::ctime(&t);
        if(!ts.empty() && ts.back()=='\n') {
            ts.pop_back();
        }
        return ts;
    }

public:
        void log(std::string &event){
            logs.push_back({currentTimeStamp(),event});
        }
        void printAll() const {
            std::cout<<"\n================ Audit Logs ==========="<<std::endl;
            for(const auto &entry : logs){
                std::cout<<"["<<entry.timestamp<<"] | "<<"["<<entry.event<<"]"<<std::endl;
            }
            std::cout<<"\n================ Logs End ===============\n";
        }
        auto count() const {
            return logs.size();
        }
};