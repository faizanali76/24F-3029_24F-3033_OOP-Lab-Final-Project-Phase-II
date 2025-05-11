#include "Stronghold.h"

Bank::Bank() {
    activeLoans = 0;
    detectedFraud = 0;
}

// Checks the treasury for any  business
void Bank::auditTreasury(Economy& economy) {
    cout << "\n==================================================\n";
    cout << "                    BANK AUDIT                     \n";
    cout << "==================================================\n";
    
    int treasury = economy.getTreasury();
    if (treasury < 0) {
        cout << "Warning: Treasury is in debt!\n";
        detectedFraud += 5;
    }
    
    if (activeLoans > treasury * 0.5) {
        cout << "Warning: Too many active loans!\n";
        detectedFraud += 3;
    }
    
    cout << "Current Loans: " << activeLoans << " gold\n";
    cout << "Detected Fraud: " << detectedFraud << " points\n";
    cout << "==================================================\n";
}

// Gives a loan to the economy
void Bank::issueLoan(Economy& economy, int amount) {
    cout << "\n==================================================\n";
    cout << "                    LOAN ISSUANCE                  \n";
    cout << "==================================================\n";
    
    if (amount <= 0) {
        cout << "Invalid loan amount!\n";
        return;
    }
    
    cout << "Current Treasury: " << economy.getTreasury() << " gold\n";
    cout << "Current Active Loans: " << activeLoans << " gold\n";
    cout << "Proposed Loan Amount: " << amount << " gold\n";
    
    if (activeLoans + amount > economy.getTreasury() * 0.5) {
        cout << "Warning: This loan would exceed the safe limit!\n";
        cout << "Maximum safe loan amount: " << (economy.getTreasury() * 0.5 - activeLoans) << " gold\n";
        cout << "Proceed anyway? (y/n): ";
        char proceed;
        cin >> proceed;
        if (proceed != 'y' && proceed != 'Y') {
            cout << "Loan cancelled.\n";
            return;
        }
    }
    
    activeLoans += amount;
    economy.receiveLoan(amount);
    cout << "Loan of " << amount << " gold issued successfully.\n";
    cout << "New Active Loans: " << activeLoans << " gold\n";
    cout << "==================================================\n";
}

// Repays part of the loan
void Bank::repayLoan(Economy& economy, int amount) {
    cout << "\n==================================================\n";
    cout << "                    LOAN REPAYMENT                 \n";
    cout << "==================================================\n";
    
    if (amount <= 0 || amount > activeLoans) {
        cout << "Invalid repayment amount!\n";
        cout << "Current Active Loans: " << activeLoans << " gold\n";
        return;
    }
    
    if (economy.getTreasury() < amount) {
        cout << "Not enough gold in treasury!\n";
        cout << "Current Treasury: " << economy.getTreasury() << " gold\n";
        return;
    }
    
    cout << "Current Active Loans: " << activeLoans << " gold\n";
    cout << "Proposed Repayment: " << amount << " gold\n";
    cout << "Confirm repayment? (y/n): ";
    char confirm;
    cin >> confirm;
    
    if (confirm == 'y' || confirm == 'Y') {
        activeLoans -= amount;
        economy.spend(amount);
        cout << "Repaid " << amount << " gold of the loan.\n";
        cout << "Remaining Loans: " << activeLoans << " gold\n";
    } else {
        cout << "Repayment cancelled.\n";
    }
    
    cout << "==================================================\n";
}

// Shows current bank stats
void Bank::showStats() const {
    cout << "\n==================================================\n";
    cout << "                    BANK OVERVIEW                  \n";
    cout << "==================================================\n";
    cout << "Active Loans: " << activeLoans << " gold\n";
    cout << "Detected Fraud: " << detectedFraud << " points\n";
    cout << "==================================================\n";
}

// Saves bank data to a file
void Bank::saveToFile() const {
    ofstream out("bank.txt");
    if (!out) {
        cout << "Error: Failed to save bank records.\n";
        return;
    }
    
    out << activeLoans << endl;
    out << detectedFraud << endl;
    out.close();
    cout << "Bank records archived successfully.\n";
}

// Loads bank data from a file
void Bank::loadFromFile() {
    ifstream in("bank.txt");
    if (!in) {
        cout << "Error: Failed to load bank records.\n";
        return;
    }
    
    in >> activeLoans >> detectedFraud;
    in.close();
    cout << "Bank records restored successfully.\n";
}
