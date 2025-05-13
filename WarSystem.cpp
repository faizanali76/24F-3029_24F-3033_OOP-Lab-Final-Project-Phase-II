#include "MultiplayerSystems.h"

WarSystem::WarSystem() : kingdomCount(0) {
    warLog.open("war_log.txt", ios::app);
}

void WarSystem::registerKingdom(const string& kingdomName, const Army& initialArmy) {
    if (kingdomCount >= MAX_KINGDOMS) {
        cout << "Maximum number of kingdoms reached!" << endl;
        return;
    }

    kingdomNames[kingdomCount] = kingdomName;
    kingdomArmies[kingdomCount] = initialArmy;
    kingdomCount++;

    warLog << "Kingdom " << kingdomName << " registered with initial army size: "
           << initialArmy.getSoldierCount() << endl;
    warLog.flush();
}

int WarSystem::getKingdomIndex(const string& kingdomName) {
    for (int i = 0; i < kingdomCount; i++) {
        if (kingdomNames[i] == kingdomName) {
            return i;
        }
    }
    return -1;
}

Army& WarSystem::getKingdomArmy(const string& kingdomName) {
    int index = getKingdomIndex(kingdomName);
    if (index == -1) {
        // Return a default army if kingdom not found
        static Army defaultArmy;
        return defaultArmy;
    }
    return kingdomArmies[index];
}

void WarSystem::declareWar(const string& attacker, const string& defender, Army& attackerArmy) {
    int defenderIndex = getKingdomIndex(defender);
    if (defenderIndex == -1) {
        cout << "Target kingdom not found!" << endl;
        return;
    }

    cout << "Do you want to attack Kingdom " << defender << "? (y/n): ";
    char response;
    cin >> response;
    
    if (response != 'y' && response != 'Y') {
        return;
    }

    // Log war declaration
    warLog << attacker << " has declared war on " << defender << endl;
    warLog.flush();

    cout << "You have declared war on " << defender << endl;
    cout << "Army morale: +10%" << endl;
    cout << "Public support: -5%" << endl;

    // Update attacker's army morale
    attackerArmy.setMorale(attackerArmy.getMorale() + 10);
    if (attackerArmy.getMorale() > 100) attackerArmy.setMorale(100);

    // Update defender's army morale
    kingdomArmies[defenderIndex].setMorale(kingdomArmies[defenderIndex].getMorale() - 5);
    if (kingdomArmies[defenderIndex].getMorale() < 0) 
        kingdomArmies[defenderIndex].setMorale(0);
}

int WarSystem::calculateBattleOutcome(const Army& attacker, const Army& defender) {
    // Simple battle calculation based on army size and morale
    int attackerStrength = attacker.getSoldierCount() * (attacker.getMorale() / 100.0);
    int defenderStrength = defender.getSoldierCount() * (defender.getMorale() / 100.0);

    // Add some randomness
    attackerStrength += rand() % 100;
    defenderStrength += rand() % 100;

    return attackerStrength - defenderStrength;
}

void WarSystem::applyBattleConsequences(Army& winner, Army& loser,
                                      ResourceManager& winnerRes, ResourceManager& loserRes) {
    // Calculate casualties
    int winnerLosses = winner.getSoldierCount() * 0.2; // 20% casualties
    int loserLosses = loser.getSoldierCount() * 0.3;   // 30% casualties

    // Update army sizes
    winner.setSoldierCount(winner.getSoldierCount() - winnerLosses);
    loser.setSoldierCount(loser.getSoldierCount() - loserLosses);

    // Update morale
    winner.setMorale(winner.getMorale() + 5);
    loser.setMorale(loser.getMorale() - 10);

    // Ensure morale stays within bounds
    if (winner.getMorale() > 100) winner.setMorale(100);
    if (loser.getMorale() < 0) loser.setMorale(0);

    // Transfer some resources from loser to winner
    int plunderedFood = loserRes.getFoodStock() * 0.2;
    int plunderedMetal = loserRes.getMetalStock() * 0.2;

    loserRes.setFoodStock(loserRes.getFoodStock() - plunderedFood);
    loserRes.setMetalStock(loserRes.getMetalStock() - plunderedMetal);
    winnerRes.setFoodStock(winnerRes.getFoodStock() + plunderedFood);
    winnerRes.setMetalStock(winnerRes.getMetalStock() + plunderedMetal);
}

void WarSystem::simulateBattle(const string& attacker, const string& defender,
                              ResourceManager& attackerRes, ResourceManager& defenderRes) {
    int attackerIndex = getKingdomIndex(attacker);
    int defenderIndex = getKingdomIndex(defender);

    if (attackerIndex == -1 || defenderIndex == -1) {
        cout << "One or both kingdoms not found!" << endl;
        return;
    }

    cout << "\n[Battle Simulation]" << endl;
    cout << "Battle between " << attacker << " and " << defender << "..." << endl;

    int battleOutcome = calculateBattleOutcome(kingdomArmies[attackerIndex], 
                                             kingdomArmies[defenderIndex]);

    if (battleOutcome > 0) {
        cout << attacker << " wins!" << endl;
        applyBattleConsequences(kingdomArmies[attackerIndex], 
                              kingdomArmies[defenderIndex],
                              attackerRes, defenderRes);
    } else {
        cout << defender << " wins!" << endl;
        applyBattleConsequences(kingdomArmies[defenderIndex],
                              kingdomArmies[attackerIndex],
                              defenderRes, attackerRes);
    }

    // Log battle results
    warLog << "Battle between " << attacker << " and " << defender << ":" << endl;
    warLog << "Winner: " << (battleOutcome > 0 ? attacker : defender) << endl;
    warLog << "Casualties:" << endl;
    warLog << attacker << " lost " << kingdomArmies[attackerIndex].getSoldierCount() * 0.2 << " soldiers" << endl;
    warLog << defender << " lost " << kingdomArmies[defenderIndex].getSoldierCount() * 0.3 << " soldiers" << endl;
    warLog << "Resources plundered: " << defenderRes.getFoodStock() * 0.2 << " Food, "
           << defenderRes.getMetalStock() * 0.2 << " Metal" << endl;
    warLog.flush();

    cout << "\nBattle Results:" << endl;
    cout << attacker << " loses " << kingdomArmies[attackerIndex].getSoldierCount() * 0.2 << " soldiers" << endl;
    cout << defender << " loses " << kingdomArmies[defenderIndex].getSoldierCount() * 0.3 << " soldiers" << endl;
    cout << "Resources plundered: " << defenderRes.getFoodStock() * 0.2 << " Food, "
         << defenderRes.getMetalStock() * 0.2 << " Metal" << endl;
}

void WarSystem::saveWarLogToFile() const {
    ofstream saveFile("war_log_save.txt");
    if (saveFile.is_open()) {
        saveFile << kingdomCount << endl;
        for (int i = 0; i < kingdomCount; i++) {
            saveFile << kingdomNames[i] << endl;
            saveFile << kingdomArmies[i].getSoldierCount() << endl;
            saveFile << kingdomArmies[i].getMorale() << endl;
        }
        saveFile.close();
    }
}

void WarSystem::loadWarLogFromFile() {
    ifstream loadFile("war_save.txt");
    if (loadFile.is_open()) {
        loadFile >> kingdomCount;
        for (int i = 0; i < kingdomCount; i++) {
            loadFile >> kingdomNames[i];
            int soldiers, morale, rations;
            loadFile >> soldiers >> morale >> rations;
            kingdomArmies[i].setSoldierCount(soldiers);
            kingdomArmies[i].setMorale(morale);
            kingdomArmies[i].setRations(rations);
        }
        loadFile.close();
    }
} 