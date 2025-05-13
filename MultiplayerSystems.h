#ifndef MULTIPLAYER_SYSTEMS_H
#define MULTIPLAYER_SYSTEMS_H

#include <iostream>
#include <fstream>
#include <string>
#include "Stronghold.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::ios;

// Constants for the game
const int MAX_KINGDOMS = 4;
const int MAX_MESSAGES = 100;
const int MAX_ALLIANCES = 10;
const int MAX_TRADES = 50;
const int MAP_SIZE = 4;

// Message types
enum MessageType {
    ALLIANCE_REQUEST,
    TRADE_OFFER,
    WAR_THREAT,
    DECEPTION
};

// Message structure
struct Message {
    string senderKingdom;
    string receiverKingdom;
    string content;
    MessageType type;
    bool isRead;
};

// Alliance structure
struct Alliance {
    string kingdom1;
    string kingdom2;
    int trustLevel;
    bool isActive;
};

// Trade structure
struct Trade {
    string offeringKingdom;
    string receivingKingdom;
    int resource1Amount;
    int resource2Amount;
    string resource1Type;
    string resource2Type;
    bool isAccepted;
};

// Map position structure
struct MapPosition {
    int x;
    int y;
};

// Communication System
class CommunicationSystem {
private:
    Message messages[MAX_MESSAGES];
    int messageCount;
    ofstream messageLog;

public:
    CommunicationSystem();
    void sendMessage(const string& sender, const string& receiver, const string& content, MessageType type);
    void displayMessages(const string& kingdom);
    void saveMessagesToFile() const;
    void loadMessagesFromFile();
};

// Alliance System
class AllianceSystem {
private:
    Alliance alliances[MAX_ALLIANCES];
    int allianceCount;
    ofstream allianceLog;

public:
    AllianceSystem();
    bool formAlliance(const string& kingdom1, const string& kingdom2);
    bool breakAlliance(const string& kingdom1, const string& kingdom2);
    void updateTrustLevel(const string& kingdom1, const string& kingdom2, int change);
    bool areAllied(const string& kingdom1, const string& kingdom2);
    void saveAlliancesToFile() const;
    void loadAlliancesFromFile();
    int getTrustLevel(const string& kingdom1, const string& kingdom2) const;
};

// Trade System
class TradeSystem {
private:
    Trade trades[MAX_TRADES];
    int tradeCount;
    ofstream tradeLog;

public:
    TradeSystem();
    void offerTrade(const string& offeringKingdom, const string& receivingKingdom,
                   const string& resource1Type, int resource1Amount,
                   const string& resource2Type, int resource2Amount);
    bool acceptTrade(int tradeId);
    void executeSmuggling(const string& kingdom, int goldAmount, int riskPercentage);
    void saveTradesToFile() const;
    void loadTradesFromFile();
};

// Map System
class MapSystem {
private:
    MapPosition kingdomPositions[MAX_KINGDOMS];
    string kingdomNames[MAX_KINGDOMS];
    int kingdomCount;
    ofstream mapLog;

public:
    MapSystem();
    void initializeKingdom(const string& kingdomName, int x, int y);
    bool moveKingdom(const string& kingdomName, int newX, int newY);
    void displayMap();
    void saveMapToFile() const;
    void loadMapFromFile();
    MapPosition getKingdomPosition(const string& kingdomName);
};

// War System
class WarSystem {
private:
    ofstream warLog;
    Army kingdomArmies[MAX_KINGDOMS];
    string kingdomNames[MAX_KINGDOMS];
    int kingdomCount;
    
    int calculateBattleOutcome(const Army& attacker, const Army& defender);
    void applyBattleConsequences(Army& winner, Army& loser, ResourceManager& winnerRes, ResourceManager& loserRes);
    int getKingdomIndex(const string& kingdomName);

public:
    WarSystem();
    void declareWar(const string& attacker, const string& defender, Army& attackerArmy);
    void simulateBattle(const string& attacker, const string& defender,
                       ResourceManager& attackerRes, ResourceManager& defenderRes);
    void saveWarLogToFile() const;
    void loadWarLogFromFile();
    void registerKingdom(const string& kingdomName, const Army& initialArmy);
    Army& getKingdomArmy(const string& kingdomName);
};

#endif // MULTIPLAYER_SYSTEMS_H 