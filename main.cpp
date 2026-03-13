#include <iostream>
#include <memory>

#include "models/Customer.hpp"
#include "models/SavingAccount.hpp"
#include "models/CurrentAccount.hpp"
#include "infrastructure/AuditLogs.hpp"
#include "security/FraudDetectionService.hpp"
#include "services/AccountService.hpp"
#include "services/LoanService.hpp"
#include "services/NotificationService.hpp"
#include "services/TransactionService.hpp"

using namespace std;
#define fastio() ios::sync_with_stdio(false); cin.tie(nullptr)

int main() {
    fastio();
    cout << "============================================\n";
    cout << "       C++ Banking System - Demo            \n";
    cout << "============================================\n\n";

    // Infrastructure
    AuditLogs               auditLogs;

    // Services
    NotificationService     notifService;
    FraudDetectionService   fraudService;
    TransactionService      txnService(fraudService,notifService,auditLogs);
    AccountService          accService(auditLogs);
    LoanService             loanService(notifService,auditLogs);

    //  Customers
    Customer Abhishek("cust-0000001","Abhishek","abhishekltr2002@gmail.com","9334905084");
    Customer Rupesh("cust-0000002","Rupesh","rupeshltr009@gmail.com","8210010101");

    cout<<"Customers Created : \n";
    cout<<Abhishek.toString()<<endl;
    cout<<Rupesh.toString()<<endl;

    
    return 0;
}