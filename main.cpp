#include <iostream>
#include <fstream>
#include "Stronghold.h"  // Your header with all class declarations
#include "MultiplayerSystems.h"

using namespace std;

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

// --- Multiplayer Kingdom Data Structure ---
struct KingdomData {
    string name;
    int x, y;
    int population;
    int armySize;
    int gold;
    int morale;
    int happiness;
    // Add more stats as needed
};

KingdomData kingdoms[MAX_KINGDOMS];
int kingdomCount = 0;
int activeKingdomIndex = 0;

// --- Alliance and War Tracking ---
bool alliances[MAX_KINGDOMS][MAX_KINGDOMS] = {false};
bool wars[MAX_KINGDOMS][MAX_KINGDOMS] = {false};

// --- Multiplayer Management Menu ---
void multiplayerManagementMenu() {
    while (true) {
        cout << "\n===============================\n";
        cout << "      MULTIPLAYER MANAGEMENT\n";
        cout << "===============================\n";
        cout << "1. Create New Kingdom\n";
        cout << "2. View All Kingdoms\n";
        cout << "3. Switch Active Kingdom\n";
        cout << "4. Return to Main Menu\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            if (kingdomCount >= MAX_KINGDOMS) {
                cout << "Maximum number of kingdoms reached!\n";
                continue;
            }
            KingdomData& k = kingdoms[kingdomCount];
            cout << "Enter kingdom name: ";
            cin.ignore();
            getline(cin, k.name);
            cout << "Choose X coordinate (0–3): ";
            cin >> k.x;
            cout << "Choose Y coordinate (0–3): ";
            cin >> k.y;
            cout << "Enter starting population: ";
            cin >> k.population;
            cout << "Enter starting army size: ";
            cin >> k.armySize;
            cout << "Enter starting gold: ";
            cin >> k.gold;
            k.morale = 50;
            k.happiness = 50;
            kingdomCount++;
            cout << "\nKingdom " << k.name << " created at position (" << k.x << "," << k.y << ").\n";
        } else if (choice == 2) {
            cout << "===== All Registered Kingdoms =====\n";
            for (int i = 0; i < kingdomCount; ++i) {
                cout << (i+1) << ". " << kingdoms[i].name << "  - Pos(" << kingdoms[i].x << "," << kingdoms[i].y << ")\n";
            }
            cout << "===================================\n";
        } else if (choice == 3) {
            cout << "Choose kingdom to control (1-" << kingdomCount << "): ";
            int idx; cin >> idx;
            if (idx >= 1 && idx <= kingdomCount) {
                activeKingdomIndex = idx-1;
                cout << "You are now controlling: " << kingdoms[activeKingdomIndex].name << "\n";
            } else {
                cout << "Invalid choice!\n";
            }
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
}

// --- Multiplayer Actions Menu (Refined) ---
void multiplayerActionsMenu() {
    while (true) {
        KingdomData& activeKingdom = kingdoms[activeKingdomIndex];
        cout << "\n===============================\n";
        cout << "       MULTIPLAYER ACTIONS\n";
        cout << "===============================\n";
        cout << "Active Kingdom: " << activeKingdom.name << "\n";
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
            cout << "Current alliances:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && alliances[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    found = true;
                }
            }
            if (!found) cout << "None\n";
        } else if (choice == 2) {
            cout << "Enter kingdom name to propose alliance: ";
            string ally; cin.ignore(); getline(cin, ally);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == ally) idx = i;
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (alliances[activeKingdomIndex][idx]) {
                cout << "Already allied!\n";
            } else {
                alliances[activeKingdomIndex][idx] = alliances[idx][activeKingdomIndex] = true;
                ofstream log("alliances_log.txt", ios::app);
                log << activeKingdom.name << " formed alliance with " << kingdoms[idx].name << endl;
                log.close();
                cout << "Alliance formed with " << kingdoms[idx].name << ".\nSaved to: alliances_log.txt\n";
            }
        } else if (choice == 3) {
            cout << "Enter kingdom name to break alliance: ";
            string ally; cin.ignore(); getline(cin, ally);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == ally) idx = i;
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (!alliances[activeKingdomIndex][idx]) {
                cout << "No alliance exists!\n";
            } else {
                alliances[activeKingdomIndex][idx] = alliances[idx][activeKingdomIndex] = false;
                ofstream log("alliances_log.txt", ios::app);
                log << activeKingdom.name << " broke alliance with " << kingdoms[idx].name << endl;
                log.close();
                cout << "Alliance broken with " << kingdoms[idx].name << ".\nSaved to: alliances_log.txt\n";
            }
        } else if (choice == 4) {
            cout << "Current wars:\n";
            bool found = false;
            for (int i = 0; i < kingdomCount; ++i) {
                if (i != activeKingdomIndex && wars[activeKingdomIndex][i]) {
                    cout << "- " << kingdoms[i].name << "\n";
                    found = true;
                }
            }
            if (!found) cout << "None\n";
        } else if (choice == 5) {
            cout << "Enter kingdom name to declare war on: ";
            string target; cin.ignore(); getline(cin, target);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == target) idx = i;
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (wars[activeKingdomIndex][idx]) {
                cout << "Already at war!\n";
            } else {
                wars[activeKingdomIndex][idx] = wars[idx][activeKingdomIndex] = true;
                ofstream log("war_log.txt", ios::app);
                log << activeKingdom.name << " declared war on " << kingdoms[idx].name << endl;
                log.close();
                cout << "\nWar declared!\n";
                cout << kingdoms[idx].name << " morale -10%\n";
                cout << activeKingdom.name << " army morale +5%\n";
                cout << "War logged to: war_log.txt\n";
            }
        } else if (choice == 6) {
            cout << "Enter kingdom name to make peace with: ";
            string target; cin.ignore(); getline(cin, target);
            int idx = -1;
            for (int i = 0; i < kingdomCount; ++i) if (kingdoms[i].name == target) idx = i;
            if (idx == -1 || idx == activeKingdomIndex) {
                cout << "Invalid kingdom!\n";
            } else if (!wars[activeKingdomIndex][idx]) {
                cout << "Not at war!\n";
            } else {
                wars[activeKingdomIndex][idx] = wars[idx][activeKingdomIndex] = false;
                ofstream log("war_log.txt", ios::app);
                log << activeKingdom.name << " made peace with " << kingdoms[idx].name << endl;
                log.close();
                cout << "Peace made with " << kingdoms[idx].name << ".\nWar log updated.\n";
            }
        } else if (choice == 7) {
            cout << "Enter recipient kingdom name: ";
            string recipient; cin.ignore(); getline(cin, recipient);
            cout << "Enter your message: ";
            string msg; getline(cin, msg);
            ofstream log("messages_log.txt", ios::app);
            log << activeKingdom.name << " to " << recipient << ": " << msg << endl;
            log.close();
            cout << "Message sent to " << recipient << ".\nSaved to: messages_log.txt\n";
        } else if (choice == 8) {
            cout << "Enter kingdom to trade with: ";
            string tradeWith; cin.ignore(); getline(cin, tradeWith);
            int offerGold, offerFood, reqSteel;
            cout << "Offer (your side) – Gold: "; cin >> offerGold;
            cout << "Offer (your side) – Food: "; cin >> offerFood;
            cout << "Request (their side) – Steel: "; cin >> reqSteel;
            cout << "\nSend this trade offer? (y/n): ";
            char yn; cin >> yn;
            if (yn == 'y' || yn == 'Y') {
                ofstream log("trade_log.txt", ios::app);
                log << activeKingdom.name << " offers Gold: " << offerGold << ", Food: " << offerFood
                    << " in exchange for Steel: " << reqSteel << " to " << tradeWith << endl;
                log.close();
                cout << "Trade offer sent to " << tradeWith << ".\nWaiting for response...\nSaved to: trade_log.txt\n";
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

    cout << "\n==================================================\n";
    cout << "                    KINGDOM CREATION               \n";
    cout << "==================================================\n";
    cout << "Enter your kingdom's name: ";
    string kingdomName;
    getline(cin, kingdomName);
    playerKingdom.setName(kingdomName);
    
    // Initialize kingdom on the map
    cout << "Choose your starting position (0-3 for both x and y):\n";
    int startX, startY;
    cout << "X coordinate: ";
    cin >> startX;
    cout << "Y coordinate: ";
    cin >> startY;
    mapSystem.initializeKingdom(kingdomName, startX, startY);
    
    // Register other kingdoms for multiplayer testing
    string otherKingdoms[] = {"NIMALA", "VALORIA", "ELDRIA"};
    Army defaultArmy;
    for (const string& k : otherKingdoms) {
        if (k != kingdomName) {
            mapSystem.initializeKingdom(k, rand()%MAP_SIZE, rand()%MAP_SIZE);
            warSystem.registerKingdom(k, defaultArmy);
            // Optionally, register with other systems if needed
        }
    }
    // Register player kingdom with war system
    warSystem.registerKingdom(kingdomName, realmForces);

    cout << "Welcome to the Kingdom of " << playerKingdom.getName() << "!\n";
    cout << "==================================================\n\n";

    int userSelection;
    bool gameActive = true;

    while (gameActive) {
        cout << "\n==================================================\n";
        cout << "                    " << playerKingdom.getName() << "                      \n";
        cout << "==================================================\n";
        cout << "1. Show Stats                                     \n";
        cout << "2. Manage People                                  \n";
        cout << "3. Manage Army                                    \n";
        cout << "4. Manage Money                                   \n";
        cout << "5. Get Resources                                  \n";
        cout << "6. Random Events                                  \n";
        cout << "7. Multiplayer Actions                           \n";
        cout << "8. Save Game                                      \n";
        cout << "9. Load Game                                      \n";
        cout << "10. Exit Game                                     \n";
        cout << "11. Multiplayer Management Menu                    \n";
        cout << "12. Multiplayer Actions Menu                        \n";
        cout << "==================================================\n";
        cout << "Enter your choice: ";
        cin >> userSelection;

        if (cin.fail() || userSelection < 1 || userSelection > 12) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\nInvalid choice! Please select between 1 and 12.\n";
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
                multiplayerActionsMenu();
                break;

            case 8:
                cout << "\nSaving Game:\n";
                saveGameState(realmCitizens, realmForces, realmEconomy,
                            realmResources, realmTreasury, commSystem,
                            allianceSystem, tradeSystem, mapSystem);
                break;

            case 9:
                cout << "\nLoading Game:\n";
                loadGameState(realmCitizens, realmForces, realmEconomy,
                            realmResources, realmTreasury, commSystem,
                            allianceSystem, tradeSystem, mapSystem);
                break;

            case 10:
                gameActive = false;
                break;

            case 11:
                multiplayerManagementMenu();
                break;

            case 12:
                multiplayerActionsMenu();
                break;
        }
    }

    delete realmRuler;
    cout << "\nGame ended. Thanks for playing!\n";
    return 0;
}
