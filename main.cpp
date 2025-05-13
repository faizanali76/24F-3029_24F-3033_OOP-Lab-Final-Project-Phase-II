#include <iostream>
#include <fstream>
#include <ctime>
#include "Stronghold.h"  
#include "MultiplayerSystems.h"

using namespace std;

void saveGameState(const Population& pop, const Army& army, const Economy& eco, 
                  const ResourceManager& res, const Bank& bank,
                  const CommunicationSystem& comm, const AllianceSystem& alliance,
                  const TradeSystem& trade, const MapSystem& map);

void loadGameState(Population& pop, Army& army, Economy& eco, 
                  ResourceManager& res, Bank& bank,
                  CommunicationSystem& comm, AllianceSystem& alliance,
                  TradeSystem& trade, MapSystem& map);

void multiplayerManagementMenu(Kingdom& playerKingdom, Population& realmCitizens, 
                             Army& realmForces, Economy& realmEconomy, 
                             WarSystem& warSystem, MapSystem& mapSystem);

void multiplayerActionsMenu(WarSystem& warSystem, AllianceSystem& allianceSystem,
                          CommunicationSystem& commSystem);

// Multiplayer Kingdom Data Structure 
struct KingdomResources {
    int gold;
    int food;
    int army;
    int materials;  // For buildings and equipment
    int population;
    int morale;
    int happiness;
};

struct KingdomData {
    string name;
    int x, y;
    KingdomResources resources;
};

KingdomData kingdoms[MAX_KINGDOMS];
int kingdomCount = 0;
int activeKingdomIndex = 0;

//  Alliance and War Tracking 
bool alliances[MAX_KINGDOMS][MAX_KINGDOMS] = {false};
bool wars[MAX_KINGDOMS][MAX_KINGDOMS] = {false};

// Helper functions
int calculateRecommendedArmy(int population);
bool isTradeFavorable(const KingdomResources& offering, const KingdomResources& requesting, const KingdomResources& kingdom);

// Random number generator using time
int getRandomNumber(int min, int max) {
    static bool initialized = false;
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }
    return min + (rand() % (max - min + 1));
}

const char* AI_RESPONSES[] = {
    "We acknowledge your message.",
    "Your message has been received.",
    "We will consider your proposal.",
    "Your words have been noted.",
    "We shall respond in due time."
};
const int NUM_RESPONSES = 5;

// Calculate initial kingdom stats based on position and surroundings
void calculateInitialStats(KingdomData& k, int x, int y, const KingdomData* existingKingdoms, int count) {
    // Base population based on position (corners and center are better)
    int basePop = 1000;
    if ((x == 0 || x == 3) && (y == 0 || y == 3)) basePop = 1500; // Corners
    else if (x == 1 && y == 1) basePop = 2000; // Center
    
    // It will adjust population based on nearby kingdoms
    for (int i = 0; i < count; i++) {
        int dist = abs(existingKingdoms[i].x - x) + abs(existingKingdoms[i].y - y);
        if (dist == 1) basePop += 500;  // Adjacent kingdoms increase population
        if (dist == 2) basePop += 200;  // Nearby kingdoms have some effect
    }
    
    // Calculate population distribution
    int peasants = basePop * 0.8 + getRandomNumber(-100, 100);
    int merchants = basePop * 0.15 + getRandomNumber(-50, 50);
    int nobles = basePop * 0.05 + getRandomNumber(-20, 20);
    
    k.resources.population = peasants + merchants + nobles;
    
    // Army based on population
    
    k.resources.army = (k.resources.population * 0.2) + getRandomNumber(-50, 50);
    if (k.resources.army < 100) k.resources.army = 100; // Minimum army size
    

    // Morale based on position and nearby kingdoms
    k.resources.morale = 50;
    if ((x == 0 || x == 3) && (y == 0 || y == 3)) k.resources.morale += 10; // Corners are defensible
    for (int i = 0; i < count; i++) {
        int dist = abs(existingKingdoms[i].x - x) + abs(existingKingdoms[i].y - y);
        if (dist == 1) k.resources.morale -= 5;  // Adjacent kingdoms reduce morale
    }
    
    // THe Economy is based on the position and trade potential
    k.resources.gold = 5000;
    int tradeRoutes = 0;
    for (int i = 0; i < count; i++) {
        int dist = abs(existingKingdoms[i].x - x) + abs(existingKingdoms[i].y - y);
        if (dist <= 2) tradeRoutes++;
    }
    k.resources.gold += tradeRoutes * 1000;
    
    // Happines is based on the initial conditions
    k.resources.happiness = 50;
    if (k.resources.morale > 60) k.resources.happiness += 10;
    if (k.resources.gold > 7000) k.resources.happiness += 10;
}

// Calculate battle outcome based on actual kingdom stats
int calculateBattleOutcome(const KingdomData& attacker, const KingdomData& defender) {
    // Base strength calculation
    double attackerStrength = attacker.resources.army * (attacker.resources.morale / 100.0);
    double defenderStrength = defender.resources.army * (defender.resources.morale / 100.0);
    
    // Position modifiers
    int attackerPos = attacker.x + attacker.y;
    int defenderPos = defender.x + defender.y;
    if (attackerPos < defenderPos) attackerStrength *= 1.1; // Attacker has better position
    else defenderStrength *= 1.1;
    
    // Random factor (10% variation)
    attackerStrength *= (0.9 + (getRandomNumber(0, 20) / 100.0));
    defenderStrength *= (0.9 + (getRandomNumber(0, 20) / 100.0));
    
    // Calculate outcome
    if (attackerStrength > defenderStrength * 1.5) return 2;  // Decisive victory
    if (attackerStrength > defenderStrength) return 1;        // Victory
    if (attackerStrength * 1.5 < defenderStrength) return -2; // Decisive defeat
    if (attackerStrength < defenderStrength) return -1;       // Defeat
    return 0;  // Draw
}

// Calculate trade value and acceptance chance
bool evaluateTradeOffer(const KingdomData& offering, const KingdomData& receiving,
                       int offerGold, int offerArmy, int reqGold, int reqArmy) {
    // Calculate relative value of the trade
    double offerValue = (offerGold + (offerArmy * 100)) * (1.0 + (offering.resources.morale / 200.0));
    double requestValue = (reqGold + (reqArmy * 100)) * (1.0 + (receiving.resources.morale / 200.0));
    
    // Consider kingdom's current situation
    double receivingNeed = 0;
    if (receiving.resources.gold < 2000) receivingNeed += 0.3;  // Needs gold
    if (receiving.resources.army < 200) receivingNeed += 0.3;  // Needs army
    
    // Calculate acceptance chance
    double valueRatio = offerValue / requestValue;
    double baseChance = 50.0;
    
    if (valueRatio > 1.2) baseChance += 20;  // Good deal
    if (valueRatio < 0.8) baseChance -= 20;  // Bad deal
    
    baseChance += receivingNeed * 20;  // More likely to accept if in need
    
    // Add some randomness
    baseChance += getRandomNumber(-10, 10);
    
    return getRandomNumber(0, 100) < baseChance;
}

// Generate dynamic AI response based on context
string generateAIResponse(const string& sender, const string& receiver,
                         MessageType type, int trustLevel, bool isAtWar) {
    string response;
    
    if (isAtWar) {
        if (type == ALLIANCE_REQUEST) {
            response = "We cannot consider an alliance while at war.";
        } else if (type == TRADE_OFFER) {
            response = "Trade during war? You must be joking.";
        } else {
            response = "Your words mean nothing to us while our armies clash.";
        }
    } else {
        if (trustLevel > 70) {
            if (type == ALLIANCE_REQUEST) {
                response = "We would be honored to join forces with you.";
            } else if (type == TRADE_OFFER) {
                response = "Your trade proposal is most welcome, trusted friend.";
            } else {
                response = "Your message is received with great respect.";
            }
        } else if (trustLevel > 40) {
            if (type == ALLIANCE_REQUEST) {
                response = "We will consider your proposal carefully.";
            } else if (type == TRADE_OFFER) {
                response = "Your offer is under consideration.";
            } else {
                response = "We acknowledge your message.";
            }
        } else {
            if (type == ALLIANCE_REQUEST) {
                response = "We need more time to build trust between our kingdoms.";
            } else if (type == TRADE_OFFER) {
                response = "Your terms are not favorable at this time.";
            } else {
                response = "We have received your message.";
            }
        }
    }
    
    return response;
}

// Saves all the game data to a file, kinda like a save point
void saveGameState(const Population& pop, const Army& army, const Economy& eco, 
                  const ResourceManager& res, const Bank& bank,
                  const CommunicationSystem& comm, const AllianceSystem& alliance,
                  const TradeSystem& trade, const MapSystem& map) {
    ofstream saveFile("game_state.txt");
    if (saveFile.is_open()) {
        // Save population data
        saveFile << pop.getTotal() << ";" << pop.getPeasantCount() << ";" 
                << pop.getMerchantCount() << ";" << pop.getNobleCount() << ";"
                << pop.getHappiness() << ";" << pop.getFoodReserves() << ";";
        
        // Save army data
        saveFile << army.getSoldierCount() << ";" << army.getMorale() << ";"
                << army.getRations() << ";";
        
        // Save economy data
        saveFile << eco.getTreasury() << ";" << eco.getTaxRate() << ";"
                << eco.getInflation() << ";";
        
        // Save resource data
        saveFile << res.getFoodStock() << ";" << res.getTimberStock() << ";"
                << res.getStoneStock() << ";" << res.getMetalStock() << ";";
        
        // Save bank data
        saveFile << bank.getActiveLoans() << ";" << bank.getDetectedFraud() << ";";
        
        // Save multiplayer systems
        comm.saveMessagesToFile();
        alliance.saveAlliancesToFile();
        trade.saveTradesToFile();
        map.saveMapToFile();
        
        saveFile.close();
        cout << "\nGame saved successfully.\n";
    } else {
        cout << "\nError: Could not save game.\n";
    }
}

// Loads the game from a save file, basically the opposite of saveGameState
void loadGameState(Population& pop, Army& army, Economy& eco, 
                  ResourceManager& res, Bank& bank,
                  CommunicationSystem& comm, AllianceSystem& alliance,
                  TradeSystem& trade, MapSystem& map) {
    ifstream loadFile("game_state.txt");
    if (loadFile.is_open()) {
        string line;
        getline(loadFile, line);
        
        // Parse the line using semicolon as delimiter
        size_t pos = 0;
        string token;
        int values[18]; // Increased array size to accommodate all values
        int index = 0;
        
        while ((pos = line.find(';')) != string::npos) {
            token = line.substr(0, pos);
            values[index++] = stoi(token);
            line.erase(0, pos + 1);
        }
        
        // Restore population
        pop.setTotal(values[0]);
        pop.setPeasantCount(values[1]);
        pop.setMerchantCount(values[2]);
        pop.setNobleCount(values[3]);
        pop.setHappiness(values[4]);
        pop.setFoodReserves(values[5]);
        
        // Restore army
        army.setSoldierCount(values[6]);
        army.setMorale(values[7]);
        army.setRations(values[8]);
        
        // Restore economy
        eco.setTreasury(values[9]);
        eco.setTaxRate(values[10]);
        eco.setInflation(values[11]);
        
        // Restore resources
        res.setFoodStock(values[12]);
        res.setTimberStock(values[13]);
        res.setStoneStock(values[14]);
        res.setMetalStock(values[15]);
        
        // Restore bank
        bank.setActiveLoans(values[16]);
        bank.setDetectedFraud(values[17]);
        
        // Load multiplayer systems
        comm.loadMessagesFromFile();
        alliance.loadAlliancesFromFile();
        trade.loadTradesFromFile();
        map.loadMapFromFile();
        
        loadFile.close();
        cout << "\nGame loaded successfully.\n";
    } else {
        cout << "\nError: Could not load game.\n";
    }
}

// --- Multiplayer Management Menu ---
void multiplayerManagementMenu(Kingdom& playerKingdom, Population& realmCitizens, 
                             Army& realmForces, Economy& realmEconomy, 
                             WarSystem& warSystem, MapSystem& mapSystem) {
    int choice;
    KingdomData k;  // For new kingdom creation
    int selectedKingdom = -1; // Index of currently selected kingdom
    do {
        cout << "\n=== Kingdom Management Menu ===" << endl;
        cout << "1. Create New Kingdom" << endl;
        cout << "2. Select Existing Kingdom" << endl;
        cout << "3. View Kingdom Stats" << endl;
        cout << "4. Return to Main Menu" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();  
        switch (choice) {
            case 1: {
                if (kingdomCount >= MAX_KINGDOMS) {
                    cout << "Maximum number of kingdoms (" << MAX_KINGDOMS << ") reached! Cannot create more kingdoms.\n";
                    break;
                }
                cout << "\n=== Kingdom Creation ===" << endl;
                cout << "Enter kingdom name: ";
                getline(cin, k.name);
                // Show current map
                cout << "\nCurrent Map:\n";
                string map[4][4];
                for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) map[i][j] = "..";
                for (int i = 0; i < kingdomCount; ++i) {
                    string abbr = kingdoms[i].name.substr(0,2);
                    map[kingdoms[i].y][kingdoms[i].x] = abbr;
                }
                for (int y = 0; y < 4; ++y) {
                    cout << "| ";
                    for (int x = 0; x < 4; ++x) cout << map[y][x] << " | ";
                    cout << "\n";
                }
                cout << "+-------------------+\n";
                cout << "Choose X coordinate (0-3): ";
                cin >> k.x;
                cout << "Choose Y coordinate (0-3): ";
                cin >> k.y;
                // Validate coordinates
                if (k.x < 0 || k.x > 3 || k.y < 0 || k.y > 3) {
                    cout << "Invalid coordinates! Please use values between 0 and 3.\n";
                    break;
                }
                // Check if position is already occupied
                bool positionOccupied = false;
                for (int i = 0; i < kingdomCount; i++) {
                    if (kingdoms[i].x == k.x && kingdoms[i].y == k.y) {
                        positionOccupied = true;
                        break;
                    }
                }
                if (positionOccupied) {
                    cout << "This position is already occupied! Please choose another position.\n";
                    break;
                }
                // Let player set initial resources
                cout << "\nSet your kingdom's initial resources:\n";
                cout << "Enter initial population (1000-5000): ";
                cin >> k.resources.population;
                if (k.resources.population < 1000) k.resources.population = 1000;
                if (k.resources.population > 5000) k.resources.population = 5000;
                int recommendedArmy = calculateRecommendedArmy(k.resources.population);
                cout << "Recommended army size: " << recommendedArmy << "\n";
                cout << "Enter initial army size (" << recommendedArmy/2 << "-" << recommendedArmy*2 << "): ";
                cin >> k.resources.army;
                if (k.resources.army < recommendedArmy/2) k.resources.army = recommendedArmy/2;
                if (k.resources.army > recommendedArmy*2) k.resources.army = recommendedArmy*2;
                cout << "Enter initial gold (1000-10000): ";
                cin >> k.resources.gold;
                if (k.resources.gold < 1000) k.resources.gold = 1000;
                if (k.resources.gold > 10000) k.resources.gold = 10000;
                cout << "Enter initial morale (50-100): ";
                cin >> k.resources.morale;
                if (k.resources.morale < 50) k.resources.morale = 50;
                if (k.resources.morale > 100) k.resources.morale = 100;
                cout << "Enter initial happiness (50-100): ";
                cin >> k.resources.happiness;
                if (k.resources.happiness < 50) k.resources.happiness = 50;
                if (k.resources.happiness > 100) k.resources.happiness = 100;
                cout << "\nKingdom Statistics:\n";
                cout << "Population: " << k.resources.population << "\n";
                cout << "Army Size: " << k.resources.army << " (Morale: " << k.resources.morale << "%)\n";
                cout << "Gold: " << k.resources.gold << "\n";
                cout << "Happiness: " << k.resources.happiness << "%\n";
                // Add to kingdoms array
                kingdoms[kingdomCount] = k;
                kingdomCount++;
                cout << "\nKingdom " << k.name << " created at position (" << k.x << "," << k.y << ").\n";
                // Register with war system
                Army defaultArmy;
                defaultArmy.setSoldierCount(k.resources.army);
                defaultArmy.setMorale(k.resources.morale);
                warSystem.registerKingdom(k.name, defaultArmy);
                selectedKingdom = kingdomCount - 1;
                break;
            }
            case 2: {
                if (kingdomCount == 0) {
                    cout << "No kingdoms available. Please create one first.\n";
                    break;
                }
                cout << "\nAvailable Kingdoms:\n";
                for (int i = 0; i < kingdomCount; ++i) {
                    cout << i+1 << ". " << kingdoms[i].name << " (" << kingdoms[i].x << "," << kingdoms[i].y << ")\n";
                }
                cout << "Select kingdom number: ";
                int sel; cin >> sel;
                if (sel < 1 || sel > kingdomCount) {
                    cout << "Invalid selection!\n";
                    break;
                }
                selectedKingdom = sel - 1;
                activeKingdomIndex = selectedKingdom;  // Update the active kingdom index
                cout << "Selected kingdom: " << kingdoms[selectedKingdom].name << "\n";
                break;
            }
            case 3: {
                if (kingdomCount == 0) {
                    cout << "No kingdoms available to view stats.\n";
                    break;
                }
                cout << "\n=== Kingdom Statistics ===\n";
                for (int i = 0; i < kingdomCount; ++i) {
                    cout << "\nKingdom: " << kingdoms[i].name << "\n";
                    cout << "Position: (" << kingdoms[i].x << "," << kingdoms[i].y << ")\n";
                    cout << "Population: " << kingdoms[i].resources.population << "\n";
                    cout << "Army Size: " << kingdoms[i].resources.army << " (Morale: " << kingdoms[i].resources.morale << "%)\n";
                    cout << "Gold: " << kingdoms[i].resources.gold << "\n";
                    cout << "Happiness: " << kingdoms[i].resources.happiness << "%\n";
                    cout << "----------------------------------------\n";
                }
                break;
            }
            case 4:
                break;
            default:
                cout << "Invalid choice!\n";
        }

        // Map after each action (always show)
        cout << "\nCurrent Map:\n";
        string map[4][4];
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) map[i][j] = "..";
        for (int i = 0; i < kingdomCount; ++i) {
            string abbr = kingdoms[i].name.substr(0,2);
            map[kingdoms[i].y][kingdoms[i].x] = abbr;
        }
        for (int y = 0; y < 4; ++y) {
            cout << "| ";
            for (int x = 0; x < 4; ++x) cout << map[y][x] << " | ";
            cout << "\n";
        }
        cout << "+-------------------+\n";
    } while (choice != 4);
}

// Multiplayer Actions Menu 
void multiplayerActionsMenu(WarSystem& warSystem, AllianceSystem& allianceSystem,
                          CommunicationSystem& commSystem) {
    while (true) {
        if (kingdomCount == 0) {
            cout << "\nNo kingdoms in multiplayer mode! Please create or join a kingdom first.\n";
            return;
        }

        KingdomData& activeKingdom = kingdoms[activeKingdomIndex];
        cout << "\n===============================\n";
        cout << "       MULTIPLAYER ACTIONS\n";
        cout << "===============================\n";
        cout << "Active Kingdom: " << activeKingdom.name << "\n";
        cout << "Population: " << activeKingdom.resources.population << "\n";
        cout << "Army Size: " << activeKingdom.resources.army << " (Morale: " << activeKingdom.resources.morale << "%)\n";
        cout << "Gold: " << activeKingdom.resources.gold << "\n";
        cout << "Happiness: " << activeKingdom.resources.happiness << "%\n\n";
        
        cout << "1. View Alliances\n";
        cout << "2. Form Alliance\n";
        cout << "3. Break Alliance\n";
        cout << "4. View Wars\n";
        cout << "5. Declare War\n";
        cout << "6. Make Peace\n";
        cout << "7. Send Message\n";
        cout << "8. Trade Resources\n";
        cout << "9. Move on Map\n";
        cout << "10. View Map\n";
        cout << "11. End Turn\n";
        cout << "12. Return to Main Menu\n";
        cout << "Enter your choice: ";
        int choice; cin >> choice;
        
        if (choice == 1) {
            cout << "\nCurrent alliances:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && alliances[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << " (Trust Level: " 
                         << allianceSystem.getTrustLevel(activeKingdom.name, kingdoms[i].name) << "%)\n";
                    found = true;
                }
            }
            if (!found) cout << "None\n";
        } else if (choice == 2) {
            cout << "\nAvailable kingdoms for alliance:\n";
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && !alliances[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    cout << "  Population: " << kingdoms[i].resources.population << "\n";
                    cout << "  Army: " << kingdoms[i].resources.army << "\n";
                    cout << "  Gold: " << kingdoms[i].resources.gold << "\n";
                }
            }
            cout << "\nEnter kingdom name to propose alliance: ";
            string ally; cin.ignore(); getline(cin, ally);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == ally) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (alliances[activeKingdomIndex][idx]) {
                cout << "Already allied!\n";
            } else {
                // Get the active and target kingdom resources
                const KingdomResources& activeRes = kingdoms[activeKingdomIndex].resources;
                const KingdomResources& targetRes = kingdoms[idx].resources;
                
                bool shouldAccept = false;
                string reason = "";
                
                // Casess
                // Case 1: Stronger kingdom proposing to weaker kingdom
                if (activeRes.army > targetRes.army && activeRes.gold > targetRes.gold) {
                    if (activeRes.army - targetRes.army > 100 && activeRes.gold - targetRes.gold > 400) {
                        shouldAccept = true;
                        reason = "Your military and economic strength makes this alliance beneficial for us.";
                    }
                }
                // Case 2: Weaker kingdom proposing to stronger kingdom
                else if (activeRes.army < targetRes.army && activeRes.gold < targetRes.gold) {
                    if (activeRes.population > targetRes.population) {
                        shouldAccept = true;
                        reason = "Your large population would be valuable to our kingdom.";
                    } else if (activeRes.army > targetRes.army || activeRes.gold > targetRes.gold) {
                        shouldAccept = true;
                        reason = "Your resources would strengthen our alliance.";
                    }
                }
                // Case 3: Mixed strengths (one higher in army, other in gold)
                else {
                    if ((activeRes.army > targetRes.army && activeRes.gold > targetRes.gold) ||
                        (activeRes.population > targetRes.population * 1.2)) {
                        shouldAccept = true;
                        reason = "Our kingdoms complement each other's strengths.";
                    }
                }
                
                if (shouldAccept) {
                    alliances[activeKingdomIndex][idx] = alliances[idx][activeKingdomIndex] = true;
                    allianceSystem.updateTrustLevel(kingdoms[activeKingdomIndex].name, kingdoms[idx].name, 20);
                    cout << "\n" << kingdoms[idx].name << " has accepted your alliance proposal!\n";
                    cout << "Reason: " << reason << "\n";
                    cout << "Trust level increased by 20%.\n";
                } else {
                    cout << "\n" << kingdoms[idx].name << " has rejected your alliance proposal.\n";
                    cout << "The current balance of power does not make this alliance beneficial.\n";
                    allianceSystem.updateTrustLevel(kingdoms[activeKingdomIndex].name, kingdoms[idx].name, -5);
                }
            }
        } else if (choice == 3) {
            cout << "\nCurrent allies:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && alliances[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    found = true;
                }
            }
            if (!found) {
                cout << "None\n";
                continue;
            }
            
            cout << "\nEnter kingdom name to break alliance: ";
            string ally; cin.ignore(); getline(cin, ally);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == ally) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (!alliances[activeKingdomIndex][idx]) {
                cout << "No alliance exists!\n";
            } else {
                alliances[activeKingdomIndex][idx] = alliances[idx][activeKingdomIndex] = false;
                allianceSystem.updateTrustLevel(activeKingdom.name, kingdoms[idx].name, -30);
                cout << "\nAlliance broken with " << kingdoms[idx].name << ".\n";
                cout << "Trust level decreased by 30%.\n";
            }
        } else if (choice == 4) {
            cout << "\nCurrent wars:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && wars[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    found = true;
                }
            }
            if (!found) cout << "None\n";
        } else if (choice == 5) {
            cout << "\nAvailable kingdoms for war:\n";
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && !wars[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    cout << "  Army: " << kingdoms[i].resources.army << " (Morale: " << kingdoms[i].resources.morale << "%)\n";
                    cout << "  Gold: " << kingdoms[i].resources.gold << "\n";
                    cout << "  Population: " << kingdoms[i].resources.population << "\n";
                }
            }
            
            cout << "\nEnter kingdom name to declare war on: ";
            string target; cin.ignore(); getline(cin, target);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == target) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (wars[activeKingdomIndex][idx]) {
                cout << "Already at war!\n";
            } else {
                // Get the active and target kingdom resources
                const KingdomResources& attackerRes = kingdoms[activeKingdomIndex].resources;
                const KingdomResources& defenderRes = kingdoms[idx].resources;
                
                // Calculate relative strengths
                double armyRatio = (double)attackerRes.army / defenderRes.army;
                double goldRatio = (double)attackerRes.gold / defenderRes.gold;
                double moraleRatio = (double)attackerRes.morale / defenderRes.morale;
                
                // Calculate population impact (affects war sustainability)
                double populationRatio = (double)attackerRes.population / defenderRes.population;
                
                // Declare war
                wars[activeKingdomIndex][idx] = wars[idx][activeKingdomIndex] = true;
                warSystem.declareWar(kingdoms[activeKingdomIndex].name, kingdoms[idx].name, 
                                   warSystem.getKingdomArmy(kingdoms[activeKingdomIndex].name));
                
                cout << "\n=== War Declared ===\n";
                cout << kingdoms[activeKingdomIndex].name << " has declared war on " << kingdoms[idx].name << "!\n\n";
                
                // Battle Outcome
                string outcome;
                string reason;
                bool attackerWins = false;
                
                // Case 1 Overwhelming military superiority (30% more army)
                if (armyRatio >= 1.3) {
                    attackerWins = true;
                    outcome = "Decisive Victory";
                    reason = "Overwhelming military superiority";
                }
                // Case 2 Significant economic advantage (35% more gold) with decent army
                else if (goldRatio >= 1.35 && armyRatio >= 1.1) {
                    attackerWins = true;
                    outcome = "Strategic Victory";
                    reason = "Superior economic resources and adequate military strength";
                }
                // Case 3 High morale advantage with decent army
                else if (moraleRatio >= 1.4 && armyRatio >= 1.05) {
                    attackerWins = true;
                    outcome = "Moral Victory";
                    reason = "Superior troop morale and adequate numbers";
                }
                // Case 4 Population advantage with decent resources
                else if (populationRatio >= 1.5 && (armyRatio >= 1.05 || goldRatio >= 1.2)) {
                    attackerWins = true;
                    outcome = "Resource Victory";
                    reason = "Superior population and adequate resources";
                }
                // Case 5 Defender wins
                else {
                    attackerWins = false;
                    outcome = "Defeat";
                    reason = "Insufficient military, economic, or population advantage";
                }
                
                // Apply battle consequences
                if (attackerWins) {
                    // Attacker gains
                    kingdoms[activeKingdomIndex].resources.morale += 15;
                    kingdoms[activeKingdomIndex].resources.gold += defenderRes.gold * 0.2; // 20% of defender's gold
                    kingdoms[activeKingdomIndex].resources.army += defenderRes.army * 0.1; // 10% of defender's army
                    
                    // Defender losses
                    kingdoms[idx].resources.morale -= 20;
                    kingdoms[idx].resources.gold *= 0.8; // Lose 20% of gold
                    kingdoms[idx].resources.army *= 0.7; // Lose 30% of army
                    kingdoms[idx].resources.population *= 0.9; // Lose 10% of population
                    
                    cout << "\nBattle Result: " << outcome << " for " << kingdoms[activeKingdomIndex].name << "!\n";
                    cout << "Reason: " << reason << "\n";
                    cout << "\nConsequences:\n";
                    cout << kingdoms[activeKingdomIndex].name << " gained:\n";
                    cout << "- 20% of " << kingdoms[idx].name << "'s gold\n";
                    cout << "- 10% of " << kingdoms[idx].name << "'s army\n";
                    cout << "- 15% morale boost\n";
                } else {
                    // Defender gains
                    kingdoms[idx].resources.morale += 10;
                    kingdoms[idx].resources.gold += attackerRes.gold * 0.1; // 10% of attacker's gold
                    
                    // Attacker losses
                    kingdoms[activeKingdomIndex].resources.morale -= 15;
                    kingdoms[activeKingdomIndex].resources.gold *= 0.9; // Lose 10% of gold
                    kingdoms[activeKingdomIndex].resources.army *= 0.8; // Lose 20% of army
                    
                    cout << "\nBattle Result: " << outcome << " for " << kingdoms[activeKingdomIndex].name << "!\n";
                    cout << "Reason: " << reason << "\n";
                    cout << "\nConsequences:\n";
                    cout << kingdoms[activeKingdomIndex].name << " lost:\n";
                    cout << "- 10% of their gold\n";
                    cout << "- 20% of their army\n";
                    cout << "- 15% morale\n";
                }
            }
        } else if (choice == 6) {
            cout << "\nCurrent wars:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && wars[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    found = true;
                }
            }
            if (!found) {
                cout << "None\n";
                continue;
            }
            
            cout << "\nEnter kingdom name to make peace with: ";
            string target; cin.ignore(); getline(cin, target);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == target) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (!wars[activeKingdomIndex][idx]) {
                cout << "Not at war!\n";
            } else {
                // Remove getWarScore and just use a random chance
                int peaceChance = 50 + getRandomNumber(-20, 20);
                if (getRandomNumber(0, 100) < peaceChance) {
                    wars[activeKingdomIndex][idx] = wars[idx][activeKingdomIndex] = false;
                    cout << "\n" << kingdoms[idx].name << " has accepted your peace proposal!\n";
                    cout << "War has ended.\n";
                } else {
                    cout << "\n" << kingdoms[idx].name << " has rejected your peace proposal.\n";
                    cout << "The war continues...\n";
                }
            }
        } else if (choice == 7) {
            cout << "\nAvailable kingdoms:\n";
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex) {
                    cout << "- " << kingdoms[i].name << "\n";
                }
            }
            
            cout << "\nEnter recipient kingdom name: ";
            string recipient; cin.ignore(); getline(cin, recipient);
            cout << "Enter your message: ";
            string msg; getline(cin, msg);
            
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == recipient) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else {
                commSystem.sendMessage(activeKingdom.name, recipient, msg, ALLIANCE_REQUEST);
                cout << "\nMessage sent to " << recipient << ".\n";
                
                // Generate dynamic response based on context
                int trustLevel = allianceSystem.getTrustLevel(activeKingdom.name, recipient);
                bool isAtWar = wars[activeKingdomIndex][idx];
                string response = generateAIResponse(activeKingdom.name, recipient,
                                                  ALLIANCE_REQUEST, trustLevel, isAtWar);
                
                cout << "\nResponse from " << recipient << ": " << response << "\n";
            }
        } else if (choice == 8) {
            cout << "\nAvailable kingdoms for trade:\n";
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex) {
                    cout << "- " << kingdoms[i].name << "\n";
                    cout << "  Gold: " << kingdoms[i].resources.gold << "\n";
                    cout << "  Food: " << kingdoms[i].resources.population << "\n";
                    cout << "  Army: " << kingdoms[i].resources.army << "\n";
                    cout << "  Materials: " << kingdoms[i].resources.population << "\n";
                }
            }
            
            cout << "\nEnter kingdom to trade with: ";
            string tradeWith; cin.ignore(); getline(cin, tradeWith);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == tradeWith) idx = i;
            
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
                continue;
            }
            
            cout << "\nYour Resources:\n";
            cout << "Gold: " << activeKingdom.resources.gold << "\n";
            cout << "Food: " << activeKingdom.resources.population << "\n";
            cout << "Army: " << activeKingdom.resources.army << "\n";
            cout << "Materials: " << activeKingdom.resources.population << "\n";
            
            cout << "\nTheir Resources:\n";
            cout << "Gold: " << kingdoms[idx].resources.gold << "\n";
            cout << "Food: " << kingdoms[idx].resources.population << "\n";
            cout << "Army: " << kingdoms[idx].resources.army << "\n";
            cout << "Materials: " << kingdoms[idx].resources.population << "\n";
            
            cout << "\nEnter your offer:\n";
            KingdomResources offering, requesting;
            
            cout << "Gold to offer: "; cin >> offering.gold;
            cout << "Food to offer: "; cin >> offering.food;
            cout << "Army units to offer: "; cin >> offering.army;
            cout << "Materials to offer: "; cin >> offering.materials;
            
            cout << "\nEnter what you want in return:\n";
            cout << "Gold to request: "; cin >> requesting.gold;
            cout << "Food to request: "; cin >> requesting.food;
            cout << "Army units to request: "; cin >> requesting.army;
            cout << "Materials to request: "; cin >> requesting.materials;
            
            cout << "\nSend this trade offer? (y/n): ";
            char yn; cin >> yn;
            if (yn == 'y' || yn == 'Y') {
                bool accepted = isTradeFavorable(offering, requesting, activeKingdom.resources);
                
                if (accepted) {
                    // Execute trade
                    activeKingdom.resources.gold -= offering.gold;
                    activeKingdom.resources.gold += requesting.gold;
                    activeKingdom.resources.population -= offering.food;
                    activeKingdom.resources.population += requesting.food;
                    activeKingdom.resources.army -= offering.army;
                    activeKingdom.resources.army += requesting.army;
                    activeKingdom.resources.population -= offering.materials;
                    activeKingdom.resources.population += requesting.materials;
                    
                    kingdoms[idx].resources.gold -= requesting.gold;
                    kingdoms[idx].resources.gold += offering.gold;
                    kingdoms[idx].resources.population -= requesting.food;
                    kingdoms[idx].resources.population += offering.food;
                    kingdoms[idx].resources.army -= requesting.army;
                    kingdoms[idx].resources.army += offering.army;
                    kingdoms[idx].resources.population -= requesting.materials;
                    kingdoms[idx].resources.population += offering.materials;
                    
                    cout << "\n" << kingdoms[idx].name << " has accepted your trade offer!\n";
                    cout << "Trade completed successfully.\n";
                    
                    // Increase trust between kingdoms
                    allianceSystem.updateTrustLevel(activeKingdom.name, kingdoms[idx].name, 5);
                } else {
                    cout << "\n" << kingdoms[idx].name << " has rejected your trade offer.\n";
                    cout << "They found the terms unfavorable.\n";
                    
                    // Slight decrease in trust
                    allianceSystem.updateTrustLevel(activeKingdom.name, kingdoms[idx].name, -2);
                }
            }
        } else if (choice == 9) {
            cout << "Current position: (" << activeKingdom.x << "," << activeKingdom.y << ")\n";
            cout << "Enter new X coordinate (0–3): "; int nx; cin >> nx;
            cout << "Enter new Y coordinate (0–3): "; int ny; cin >> ny;
            activeKingdom.x = nx; activeKingdom.y = ny;
            ofstream log("map_log.txt", ios::app);
            log << activeKingdom.name << " moved to (" << nx << "," << ny << ")" << endl;
            log.close();
            cout << "\nKingdom " << activeKingdom.name << " moved to (" << nx << "," << ny << ").\nUpdated in: map_log.txt\n";
        } else if (choice == 10) {
            cout << "Current Map:\n+-------------------+\n";
            string map[4][4] = {".."};
            for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) map[i][j] = "..";
            for (int i = 0; i < kingdomCount; ++i) {
                string abbr = kingdoms[i].name.substr(0,2);
                map[kingdoms[i].y][kingdoms[i].x] = abbr;
            }
            for (int y = 0; y < 4; ++y) {
                cout << "| ";
                for (int x = 0; x < 4; ++x) cout << map[y][x] << " | ";
                cout << "\n";
            }
            cout << "+-------------------+\n\nLegend:\n";
            for (int i = 0; i < kingdomCount; ++i) {
                cout << kingdoms[i].name.substr(0,2) << " - " << kingdoms[i].name << " (" << kingdoms[i].x << "," << kingdoms[i].y << ")\n";
            }
        } else if (choice == 11) {
            cout << "End turn for " << activeKingdom.name << "? (y/n): ";
            char yn; cin >> yn;
            if (yn == 'y' || yn == 'Y') {
                activeKingdomIndex = (activeKingdomIndex + 1) % kingdomCount;
                cout << "\nNow controlling: " << kingdoms[activeKingdomIndex].name << "\n";
                break;
            }
        } else if (choice == 12) {
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
}

// Based on Population Army Recommendation size
int calculateRecommendedArmy(int population) {
    return population / 5; // 20% of population as recommended army size
}

// Calculate trade value of different resources
struct TradeValue {
    static const int GOLD_VALUE = 1;      // Base value
    static const int FOOD_VALUE = 2;      // Food is worth more than gold
    static const int ARMY_VALUE = 100;    // Each soldier is worth 100 gold
    static const int MATERIALS_VALUE = 5; // Materials are worth 5x gold
};

// Calculate if a trade is favorable
bool isTradeFavorable(const KingdomResources& offering, const KingdomResources& requesting, const KingdomResources& kingdom) {
    // Calculate total value of offered resources
    int offeredValue = offering.gold + (offering.food * 2) + (offering.army * 3) + (offering.materials * 1.5);
    
    // Calculate total value of requested resources
    int requestedValue = requesting.gold + (requesting.food * 2) + (requesting.army * 3) + (requesting.materials * 1.5);
    
    // Consider kingdom's needs
    double needMultiplier = 1.0;
    if (kingdom.food < 1000) needMultiplier += 0.5;
    if (kingdom.army < 100) needMultiplier += 0.3;
    if (kingdom.materials < 500) needMultiplier += 0.2;
    
    return (offeredValue * needMultiplier) >= requestedValue;
}

int main() {
    ResourceManager realmResources;
    Economy realmEconomy;
    Population realmCitizens;
    Army realmForces;
    Leader* realmRuler = new King();
    EventManager realmEvents;
    Bank realmTreasury;
    Kingdom playerKingdom;

    // New multiplayer systems
    CommunicationSystem commSystem;
    AllianceSystem allianceSystem;
    TradeSystem tradeSystem;
    MapSystem mapSystem;
    WarSystem warSystem;

    int userSelection;
    bool gameActive = true;

    while (gameActive) {
        cout << "\n==================================================\n";
        cout << "                    KINGDOM MANAGEMENT            \n";
        cout << "==================================================\n";
        cout << "1. Show Stats                                     \n";
        cout << "2. Manage People                                  \n";
        cout << "3. Manage Army                                    \n";
        cout << "4. Manage Money                                   \n";
        cout << "5. Get Resources                                  \n";
        cout << "6. Random Events                                  \n";
        cout << "7. Multiplayer Management Menu                    \n";
        cout << "8. Multiplayer Actions Menu                       \n";
        cout << "9. Save Game                                      \n";
        cout << "10. Load Game                                     \n";
        cout << "11. Exit Game                                     \n";
        cout << "==================================================\n";
        cout << "Enter your choice: ";
        cin >> userSelection;

        if (cin.fail() || userSelection < 1 || userSelection > 11) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\nInvalid choice! Please select between 1 and 11.\n";
            continue;
        }

        // Handle the player's choice
        switch (userSelection) {
            case 1:
                cout << "\nCurrent Stats:\n";
                realmCitizens.showStats();
                realmForces.showStats();
                realmEconomy.showStats();
                realmResources.showStats();
                realmTreasury.showStats();
                break;

            case 2:
                cout << "\nManaging People:\n";
                realmCitizens.simulate();
                break;

            case 3:
                cout << "\nManaging Army:\n";
                realmForces.recruitAndTrain(realmCitizens);
                break;

            case 4:
                cout << "\nManaging Money:\n";
                cout << "==================================================\n";
                cout << "1. Collect Taxes                                  \n";
                cout << "2. Get a Loan                                     \n";
                cout << "3. Repay Loan                                     \n";
                cout << "4. Audit Treasury                                 \n";
                cout << "==================================================\n";
                cout << "Choose an option: ";
                int moneyChoice;
                cin >> moneyChoice;
                
                switch (moneyChoice) {
                    case 1:
                        realmEconomy.taxPopulation(realmCitizens);
                        break;
                    case 2:
                        cout << "Enter loan amount: ";
                        int loanAmount;
                        cin >> loanAmount;
                        realmTreasury.issueLoan(realmEconomy, loanAmount);
                        break;
                    case 3:
                        cout << "Enter repayment amount: ";
                        int repayAmount;
                        cin >> repayAmount;
                        realmTreasury.repayLoan(realmEconomy, repayAmount);
                        break;
                    case 4:
                        realmTreasury.auditTreasury(realmEconomy);
                        break;
                    default:
                        cout << "Invalid choice!\n";
                }
                break;

            case 5:
                cout << "\nGetting Resources:\n";
                cout << "==================================================\n";
                cout << "1. Get Food\n";
                cout << "2. Get Wood\n";
                cout << "3. Get Stone\n";
                cout << "4. Get Steel\n";
                cout << "==================================================\n";
                cout << "Choose resource to get: ";
                int resourceChoice;
                cin >> resourceChoice;
                if (resourceChoice >= 1 && resourceChoice <= 4) {
                    realmResources.manage();
                } else {
                    cout << "\nInvalid choice!\n";
                }
                break;

            case 6:
                cout << "\nRandom Events:\n";
                realmEvents.trigger(realmCitizens, realmForces, realmEconomy, realmResources);
                break;

            case 7:
                multiplayerManagementMenu(playerKingdom, realmCitizens, realmForces, realmEconomy, warSystem, mapSystem);
                break;

            case 8:
                multiplayerActionsMenu(warSystem, allianceSystem, commSystem);
                break;

            case 9:
                cout << "\nSaving Game:\n";
                saveGameState(realmCitizens, realmForces, realmEconomy,
                            realmResources, realmTreasury, commSystem,
                            allianceSystem, tradeSystem, mapSystem);
                break;

            case 10:
                cout << "\nLoading Game:\n";
                loadGameState(realmCitizens, realmForces, realmEconomy,
                            realmResources, realmTreasury, commSystem,
                            allianceSystem, tradeSystem, mapSystem);
                break;

            case 11:
                gameActive = false;
                break;
        }
    }

    delete realmRuler;
    cout << "\nGame ended. Thanks for playing!\n";
    return 0;
}
