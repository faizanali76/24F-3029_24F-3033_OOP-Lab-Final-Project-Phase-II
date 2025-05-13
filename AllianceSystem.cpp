#include "MultiplayerSystems.h"

AllianceSystem::AllianceSystem() : allianceCount(0) {
    allianceLog.open("alliances_log.txt", ios::app);
}

bool AllianceSystem::formAlliance(const string& kingdom1, const string& kingdom2) {
    if (allianceCount >= MAX_ALLIANCES) {
        cout << "Maximum number of alliances reached!" << endl;
        return false;
    }

    for (int i = 0; i < allianceCount; i++) {
        if ((alliances[i].kingdom1 == kingdom1 && alliances[i].kingdom2 == kingdom2) ||
            (alliances[i].kingdom1 == kingdom2 && alliances[i].kingdom2 == kingdom1)) {
            cout << "Alliance already exists between " << kingdom1 << " and " << kingdom2 << endl;
            return false;
        }
    }

    // Create new alliance
    alliances[allianceCount].kingdom1 = kingdom1;
    alliances[allianceCount].kingdom2 = kingdom2;
    alliances[allianceCount].trustLevel = 50;
    alliances[allianceCount].isActive = true;

    allianceLog << "Alliance formed between " << kingdom1 << " and " << kingdom2 << endl;
    allianceLog.flush();

    allianceCount++;
    return true;
}

bool AllianceSystem::breakAlliance(const string& kingdom1, const string& kingdom2) {
    for (int i = 0; i < allianceCount; i++) {
        if ((alliances[i].kingdom1 == kingdom1 && alliances[i].kingdom2 == kingdom2) ||
            (alliances[i].kingdom1 == kingdom2 && alliances[i].kingdom2 == kingdom1)) {
            
            if (alliances[i].isActive) {
                alliances[i].isActive = false;
                
                allianceLog << "Alliance broken between " << kingdom1 << " and " << kingdom2 << endl;
                allianceLog.flush();

                cout << "Alliance broken between " << kingdom1 << " and " << kingdom2 << endl;
                cout << "Public reaction: -10% happiness" << endl;
                cout << "Military morale: -5%" << endl;
                
                return true;
            }
        }
    }
    return false;
}

void AllianceSystem::updateTrustLevel(const string& kingdom1, const string& kingdom2, int change) {
    for (int i = 0; i < allianceCount; i++) {
        if ((alliances[i].kingdom1 == kingdom1 && alliances[i].kingdom2 == kingdom2) ||
            (alliances[i].kingdom1 == kingdom2 && alliances[i].kingdom2 == kingdom1)) {
            
            alliances[i].trustLevel += change;
            
            if (alliances[i].trustLevel > 100) alliances[i].trustLevel = 100;
            if (alliances[i].trustLevel < 0) alliances[i].trustLevel = 0;

            allianceLog << "Trust level between " << kingdom1 << " and " << kingdom2 
                       << " changed by " << change << " to " << alliances[i].trustLevel << endl;
            allianceLog.flush();
            
            break;
        }
    }
}

bool AllianceSystem::areAllied(const string& kingdom1, const string& kingdom2) {
    for (int i = 0; i < allianceCount; i++) {
        if ((alliances[i].kingdom1 == kingdom1 && alliances[i].kingdom2 == kingdom2) ||
            (alliances[i].kingdom1 == kingdom2 && alliances[i].kingdom2 == kingdom1)) {
            return alliances[i].isActive;
        }
    }
    return false;
}

void AllianceSystem::saveAlliancesToFile() const {
    ofstream saveFile("alliances_save.txt");
    if (saveFile.is_open()) {
        saveFile << allianceCount << endl;
        for (int i = 0; i < allianceCount; i++) {
            saveFile << alliances[i].kingdom1 << endl;
            saveFile << alliances[i].kingdom2 << endl;
            saveFile << alliances[i].trustLevel << endl;
            saveFile << alliances[i].isActive << endl;
        }
        saveFile.close();
    }
}

void AllianceSystem::loadAlliancesFromFile() {
    ifstream loadFile("alliances_save.txt");
    if (loadFile.is_open()) {
        loadFile >> allianceCount;
        for (int i = 0; i < allianceCount; i++) {
            loadFile >> alliances[i].kingdom1;
            loadFile >> alliances[i].kingdom2;
            loadFile >> alliances[i].trustLevel;
            loadFile >> alliances[i].isActive;
        }
        loadFile.close();
    }
}

int AllianceSystem::getTrustLevel(const string& kingdom1, const string& kingdom2) const {
    for (int i = 0; i < allianceCount; i++) {
        if ((alliances[i].kingdom1 == kingdom1 && alliances[i].kingdom2 == kingdom2) ||
            (alliances[i].kingdom1 == kingdom2 && alliances[i].kingdom2 == kingdom1)) {
            return alliances[i].trustLevel;
        }
    }
    return 50; 
} 