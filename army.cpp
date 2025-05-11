#include "Stronghold.h"

Army::Army() {
    soldierCount = 20;
    troopMorale = 70;       // 0–100 scale
    militaryRations = 100;  // units of food for the army
}

// Recruits new soldiers and trains them, costs food and gold
void Army::recruitAndTrain(Population& pop) {
    cout << "\n==================================================\n";
    cout << "                    ARMY MANAGEMENT               \n";
    cout << "==================================================\n";
    
    if (pop.getTotal() < 100) {
        cout << "Not enough population to recruit from!\n";
        return;
    }

    cout << "Current Army Size: " << soldierCount << "\n";
    cout << "Current Morale: " << troopMorale << "%\n";
    cout << "Maximum possible recruits: " << (pop.getTotal() * 0.2 - soldierCount) << "\n";
    
    cout << "How many soldiers would you like to recruit? (0 to cancel): ";
    int newRecruits;
    cin >> newRecruits;
    
    if (newRecruits <= 0) {
        cout << "Recruitment cancelled.\n";
        return;
    }
    
    if (newRecruits + soldierCount > pop.getTotal() * 0.2) {
        cout << "Warning: That would make the army too large for the population!\n";
        cout << "Maximum allowed recruits: " << (pop.getTotal() * 0.2 - soldierCount) << "\n";
        return;
    }
    
    int foodNeeded = newRecruits * 5;
    int goldNeeded = newRecruits * 10;
    
    cout << "Training " << newRecruits << " new soldiers will require:\n";
    cout << "- " << foodNeeded << " units of food\n";
    cout << "- " << goldNeeded << " gold coins\n";
    
    cout << "Proceed with recruitment? (y/n): ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        soldierCount += newRecruits;
        troopMorale += 5; // Training boosts morale
        
        if (soldierCount > pop.getTotal() * 0.2) {
            cout << "Warning: Army size is too large for population!\n";
            troopMorale -= 10;
        }
        
        cout << "Recruitment successful!\n";
        cout << "New Army Size: " << soldierCount << "\n";
        cout << "New Morale: " << troopMorale << "%\n";
    } else {
        cout << "Recruitment cancelled.\n";
    }
    
    cout << "==================================================\n";
}

// Shows current army stats
void Army::showStats() const {
    cout << "\n==================================================\n";
    cout << "                     MILITARY OVERVIEW                     \n";
    cout << "==================================================\n";
    cout << "Active Forces: " << soldierCount << " soldiers\n";
    cout << "Troop Morale: " << troopMorale << "%\n";
    cout << "Military Rations: " << militaryRations << " units\n";
    cout << "==================================================\n";
}

// Saves army data to a file
void Army::saveToFile() const {
    ofstream out("army.txt");
    if (!out) {
        cout << "Error: Failed to save military records.\n";
        return;
    }

    out << soldierCount << ";" << troopMorale << ";" << militaryRations << ";";
    out.close();
    cout << "Military records archived successfully.\n";
}

// Loads army data from a file
void Army::loadFromFile() {
    ifstream in("army.txt");
    if (!in) {
        cout << "Error: Failed to load military records.\n";
        return;
    }

    string line;
    getline(in, line);
    size_t pos = 0;
    string token;
    int values[3];
    int index = 0;
    
    while ((pos = line.find(';')) != string::npos) {
        token = line.substr(0, pos);
        values[index++] = stoi(token);
        line.erase(0, pos + 1);
    }
    
    soldierCount = values[0];
    troopMorale = values[1];
    militaryRations = values[2];
    
    in.close();
    cout << "Military records restored successfully.\n";
}

// Lowers morale when bad stuff happens
void Army::lowerMorale(int amount) {
    troopMorale -= amount;
    if (troopMorale < 0) troopMorale = 0;
}
