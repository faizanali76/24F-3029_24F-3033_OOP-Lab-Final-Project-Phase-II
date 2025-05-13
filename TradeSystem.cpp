#include "MultiplayerSystems.h"

string getResourceName(const string& code) {
    if (code == "food" || code == "1" || code == "Food") return "Food";
    if (code == "wood" || code == "2" || code == "Wood") return "Wood";
    if (code == "stone" || code == "3" || code == "Stone") return "Stone";
    if (code == "steel" || code == "4" || code == "Steel") return "Steel";
    if (code == "gold" || code == "5" || code == "Gold") return "Gold";
    return code;
}

TradeSystem::TradeSystem() : tradeCount(0) {
    tradeLog.open("market_log.txt", ios::app);
}

void TradeSystem::offerTrade(const string& offeringKingdom, const string& receivingKingdom,
                           const string& resource1Type, int resource1Amount,
                           const string& resource2Type, int resource2Amount) {
    if (tradeCount >= MAX_TRADES) {
        cout << "Maximum number of trades reached!" << endl;
        return;
    }

    trades[tradeCount].offeringKingdom = offeringKingdom;
    trades[tradeCount].receivingKingdom = receivingKingdom;
    trades[tradeCount].resource1Type = resource1Type;
    trades[tradeCount].resource1Amount = resource1Amount;
    trades[tradeCount].resource2Type = resource2Type;
    trades[tradeCount].resource2Amount = resource2Amount;
    trades[tradeCount].isAccepted = false;

    // Log the trade offer
    tradeLog << offeringKingdom << " offers " << resource1Amount << " " << getResourceName(resource1Type)
             << " in exchange for " << resource2Amount << " " << getResourceName(resource2Type)
             << " to " << receivingKingdom << endl;
    tradeLog.flush();

    cout << offeringKingdom << " offers " << resource1Amount << " " << getResourceName(resource1Type)
         << " in exchange for " << resource2Amount << " " << getResourceName(resource2Type)
         << " to " << receivingKingdom << endl;
    cout << "Send trade offer? (y/n): ";

    char response;
    cin >> response;
    if (response == 'y' || response == 'Y') {
        tradeCount++;
    }
}

bool TradeSystem::acceptTrade(int tradeId) {
    if (tradeId < 0 || tradeId >= tradeCount) {
        cout << "Invalid trade ID!" << endl;
        return false;
    }

    if (trades[tradeId].isAccepted) {
        cout << "Trade already completed!" << endl;
        return false;
    }

    trades[tradeId].isAccepted = true;

    // Log the completed trade
    tradeLog << "Trade completed between " << trades[tradeId].offeringKingdom
             << " and " << trades[tradeId].receivingKingdom << endl;
    tradeLog << "Resources exchanged:" << endl;
    tradeLog << trades[tradeId].offeringKingdom << ": -" << trades[tradeId].resource1Amount
             << " " << trades[tradeId].resource1Type << ", +" << trades[tradeId].resource2Amount
             << " " << trades[tradeId].resource2Type << endl;
    tradeLog << trades[tradeId].receivingKingdom << ": -" << trades[tradeId].resource2Amount
             << " " << trades[tradeId].resource2Type << ", +" << trades[tradeId].resource1Amount
             << " " << trades[tradeId].resource1Type << endl;
    tradeLog.flush();

    cout << "Trade completed successfully." << endl;
    cout << "Resources updated:" << endl;
    cout << trades[tradeId].offeringKingdom << ": -" << trades[tradeId].resource1Amount
         << " " << trades[tradeId].resource1Type << ", +" << trades[tradeId].resource2Amount
         << " " << trades[tradeId].resource2Type << endl;
    cout << trades[tradeId].receivingKingdom << ": -" << trades[tradeId].resource2Amount
         << " " << trades[tradeId].resource2Type << ", +" << trades[tradeId].resource1Amount
         << " " << trades[tradeId].resource1Type << endl;

    return true;
}

void TradeSystem::executeSmuggling(const string& kingdom, int goldAmount, int riskPercentage) {
    cout << "Do you want to smuggle " << goldAmount << " gold into " << kingdom
         << " (Risk: " << riskPercentage << "%)? (y/n): ";

    char response;
    cin >> response;
    if (response != 'y' && response != 'Y') {
        return;
    }

    // Simple random chance based on risk percentage
    int randomChance = rand() % 100;
    if (randomChance < riskPercentage) {
        cout << "Smuggling failed! You were caught." << endl;
        cout << "Corruption level: +5%" << endl;
        cout << "Gold lost: " << goldAmount << endl;

        tradeLog << "Smuggling attempt failed for " << kingdom << endl;
        tradeLog << "Gold lost: " << goldAmount << endl;
        tradeLog << "Corruption increased by 5%" << endl;
    } else {
        cout << "Smuggling successful!" << endl;
        cout << "Gold transferred: " << goldAmount << endl;

        tradeLog << "Successful smuggling to " << kingdom << endl;
        tradeLog << "Gold transferred: " << goldAmount << endl;
    }
    tradeLog.flush();
}

void TradeSystem::saveTradesToFile() const {
    ofstream saveFile("trades_save.txt");
    if (saveFile.is_open()) {
        saveFile << tradeCount << endl;
        for (int i = 0; i < tradeCount; i++) {
            saveFile << trades[i].offeringKingdom << endl;
            saveFile << trades[i].receivingKingdom << endl;
            saveFile << trades[i].resource1Type << endl;
            saveFile << trades[i].resource1Amount << endl;
            saveFile << trades[i].resource2Type << endl;
            saveFile << trades[i].resource2Amount << endl;
            saveFile << trades[i].isAccepted << endl;
        }
        saveFile.close();
    }
}

void TradeSystem::loadTradesFromFile() {
    ifstream loadFile("trades_save.txt");
    if (loadFile.is_open()) {
        loadFile >> tradeCount;
        for (int i = 0; i < tradeCount; i++) {
            loadFile >> trades[i].offeringKingdom;
            loadFile >> trades[i].receivingKingdom;
            loadFile >> trades[i].resource1Type;
            loadFile >> trades[i].resource1Amount;
            loadFile >> trades[i].resource2Type;
            loadFile >> trades[i].resource2Amount;
            loadFile >> trades[i].isAccepted;
        }
        loadFile.close();
    }
} 