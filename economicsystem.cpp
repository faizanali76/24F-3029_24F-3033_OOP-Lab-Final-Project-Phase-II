#include "Stronghold.h"

Economy::Economy() {
    stateTreasury = 1000;
    currentTaxRate = 0.1;
    marketInflation = 0.0;
}

// Collects taxes from the population based on their class
void Economy::taxPopulation(const Population& pop) {
    cout << "\n==================================================\n";
    cout << "                    TAX COLLECTION                 \n";
    cout << "==================================================\n";
    
    cout << "Current Tax Rate: " << (currentTaxRate * 100) << "%\n";
    cout << "Current Treasury: " << stateTreasury << " gold\n";
    
    cout << "Would you like to change the tax rate? (y/n): ";
    char changeRate;
    cin >> changeRate;
    
    if (changeRate == 'y' || changeRate == 'Y') {
        cout << "Enter new tax rate (0-100%): ";
        float newRate;
        cin >> newRate;
        
        if (newRate >= 0 && newRate <= 100) {
            currentTaxRate = newRate / 100.0;
            cout << "Tax rate set to " << newRate << "%\n";
        } else {
            cout << "Invalid tax rate! Keeping current rate.\n";
        }
    }
    
    cout << "\nWould you like to collect taxes? (y/n): ";
    char collect;
    cin >> collect;
    
    if (collect == 'y' || collect == 'Y') {
        int peasantTax = pop.getPeasantCount() * 2;
        int merchantTax = pop.getMerchantCount() * 5;
        int nobleTax = pop.getNobleCount() * 10;
        
        int totalTax = (peasantTax + merchantTax + nobleTax) * currentTaxRate;
        stateTreasury += totalTax;
        
        cout << "\nTax Collection Results:\n";
        cout << "- Peasant Taxes: " << (peasantTax * currentTaxRate) << " gold\n";
        cout << "- Merchant Taxes: " << (merchantTax * currentTaxRate) << " gold\n";
        cout << "- Noble Taxes: " << (nobleTax * currentTaxRate) << " gold\n";
        cout << "Total Collected: " << totalTax << " gold\n";
        cout << "New Treasury: " << stateTreasury << " gold\n";
    } else {
        cout << "Tax collection cancelled.\n";
    }
    
    cout << "==================================================\n";
}

// Spends money from the treasury
void Economy::spend(int amount) {
    if (amount > stateTreasury) {
        cout << "Not enough gold in treasury!\n";
        return;
    }
    
    stateTreasury -= amount;
    cout << "Spent " << amount << " gold\n";
    cout << "Remaining Treasury: " << stateTreasury << " gold\n";
}

void Economy::showStats() const {
    cout << "\n==================================================\n";
    cout << "                    ECONOMY OVERVIEW               \n";
    cout << "==================================================\n";
    cout << "Treasury: " << stateTreasury << " gold\n";
    cout << "Tax Rate: " << (currentTaxRate * 100) << "%\n";
    cout << "Inflation: " << (marketInflation * 100) << "%\n";
    cout << "==================================================\n";
}

void Economy::saveToFile() const {
    ofstream out("economy.txt");
    if (!out) {
        cout << "Error: Failed to save economy records.\n";
        return;
    }
    
    out << stateTreasury << endl;
    out << currentTaxRate << endl;
    out << marketInflation << endl;
    out.close();
    cout << "Economy records archived successfully.\n";
}

void Economy::loadFromFile() {
    ifstream in("economy.txt");
    if (!in) {
        cout << "Error: Failed to load economy records.\n";
        return;
    }
    
    in >> stateTreasury >> currentTaxRate >> marketInflation;
    in.close();
    cout << "Economy records restored successfully.\n";
}

int Economy::getTreasury() const {
    return stateTreasury;
}

// Adds money to treasury (used for loans)
void Economy::receiveLoan(int amount) {
    stateTreasury += amount;
    cout << "Received " << amount << " gold loan\n";
}

