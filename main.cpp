#include <iostream>
#include <memory>

#include "models/Customer.hpp"
#include "models/SavingAccount.hpp"
#include "models/CurrentAccount.hpp"
#include "services/AccountService.hpp"
#include "services/TransactionService.hpp"
#include "services/LoanService.hpp"
#include "services/NotificationService.hpp"
#include "security/FraudDetectionService.hpp"
#include "infrastructure/AuditLogs.hpp"
using namespace std;
#define fastio() ios::sync_with_stdio(false); cin.tie(nullptr)

int main() {
    fastio();
    cout << "============================================\n";
    cout << "       C++ Banking System - Demo            \n";
    cout << "============================================\n\n";
    return 0;
}