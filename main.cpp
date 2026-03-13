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

    // Open Accounts
    cout<<"\n===========Opening Accounts========="<<endl;
    auto AbhishekSaving = accService.openSavingAccount(Abhishek,5500.0);
    auto RupeshCurrent = accService.openCurrentAccount(Rupesh,3000.0);
    auto RupeshSaving = accService.openSavingAccount(Rupesh,8000.0);
    accService.listAll();

    // Deposit
    std::cout<<"\n=======Deposits=========\n"<<endl;
    txnService.deposit(*AbhishekSaving,500.0);
    txnService.deposit(*RupeshSaving,600.0);
    txnService.deposit(*RupeshCurrent,500.0);

    // Withdrawl
    std::cout<<"\n========Withdrawal========\n"<<endl;
    txnService.withdraw(*AbhishekSaving,500.0);

    // Transfer
    cout<<"\n============Transfer============\n"<<endl;
    txnService.transfer(*AbhishekSaving,*RupeshSaving,800.0);

    // Apply Saving Interest
    cout<<"\n=============Apply Interest=============\n"<<endl;
    AbhishekSaving->applyIntrest();

    // Large Transaction - Fraud Detection
    cout<<"\n=============Fraud Detection==============\n"<<endl;
    txnService.deposit(*RupeshSaving,12500.0);

    // Loan Processing
    cout<<"\n===========Loan Processing==============\n"<<endl;
    LoanApplication app1 {
        "LOAN-2024-001",
        Rupesh,
        50000.0,
        15
    };
    loanService.processLoan(app1,*RupeshSaving);

    // Overdraft Test
    std::cout << "\n--- Overdraft Test (CurrentAccount) ---\n";
    try {
        txnService.withdraw(*RupeshCurrent,20000.0);
    } catch(const std::runtime_error &e) {
        std::cout<<"[Caught]" << e.what()<<endl;
    }

    // Final Balance
    cout<<"Final Account Balance :"<<endl;
    accService.listAll();

    auditLogs.printAll();
    return 0;
}