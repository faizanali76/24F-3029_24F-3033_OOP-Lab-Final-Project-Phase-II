#include "MultiplayerSystems.h"

MapSystem::MapSystem() : kingdomCount(0) {
    mapLog.open("map_log.txt", ios::app);
}

void MapSystem::initializeKingdom(const string& kingdomName, int x, int y) {
    if (kingdomCount >= MAX_KINGDOMS) {
        cout << "Maximum number of kingdoms reached!" << endl;
        return;
    }

    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE) {
        cout << "Invalid map coordinates!" << endl;
        return;
    }

    // Check if position is already occupied
    for (int i = 0; i < kingdomCount; i++) {
        if (kingdomPositions[i].x == x && kingdomPositions[i].y == y) {
            cout << "Position already occupied!" << endl;
            return;
        }
    }

    kingdomNames[kingdomCount] = kingdomName;
    kingdomPositions[kingdomCount].x = x;
    kingdomPositions[kingdomCount].y = y;

    // Log kingdom initialization
    mapLog << "Kingdom " << kingdomName << " initialized at position (" << x << "," << y << ")" << endl;
    mapLog.flush();

    kingdomCount++;
}

bool MapSystem::moveKingdom(const string& kingdomName, int newX, int newY) {
    if (newX < 0 || newX >= MAP_SIZE || newY < 0 || newY >= MAP_SIZE) {
        cout << "Invalid map coordinates!" << endl;
        return false;
    }

    // Find kingdom
    int kingdomIndex = -1;
    for (int i = 0; i < kingdomCount; i++) {
        if (kingdomNames[i] == kingdomName) {
            kingdomIndex = i;
            break;
        }
    }

    if (kingdomIndex == -1) {
        cout << "Kingdom not found!" << endl;
        return false;
    }

    // Check if new position is occupied
    for (int i = 0; i < kingdomCount; i++) {
        if (i != kingdomIndex && kingdomPositions[i].x == newX && kingdomPositions[i].y == newY) {
            cout << "Position already occupied!" << endl;
            return false;
        }
    }

    // Log the movement
    mapLog << kingdomName << " moved from (" << kingdomPositions[kingdomIndex].x << ","
           << kingdomPositions[kingdomIndex].y << ") to (" << newX << "," << newY << ")" << endl;
    mapLog.flush();

    kingdomPositions[kingdomIndex].x = newX;
    kingdomPositions[kingdomIndex].y = newY;

    return true;
}

void MapSystem::displayMap() {
    cout << "\nCurrent Map:\n";
    cout << "+-------------------+\n";
    
    for (int y = 0; y < MAP_SIZE; y++) {
        cout << "| ";
        for (int x = 0; x < MAP_SIZE; x++) {
            bool kingdomFound = false;
            for (int k = 0; k < kingdomCount; k++) {
                if (kingdomPositions[k].x == x && kingdomPositions[k].y == y) {
                    cout << kingdomNames[k].substr(0, 2) << " | ";
                    kingdomFound = true;
                    break;
                }
            }
            if (!kingdomFound) {
                cout << ".. | ";
            }
        }
        cout << "\n";
    }
    
    cout << "+-------------------+\n";
    cout << "\nLegend:\n";
    for (int i = 0; i < kingdomCount; i++) {
        cout << kingdomNames[i] << " is in position (" 
             << kingdomPositions[i].x << "," << kingdomPositions[i].y << ")\n";
    }
}

MapPosition MapSystem::getKingdomPosition(const string& kingdomName) {
    for (int i = 0; i < kingdomCount; i++) {
        if (kingdomNames[i] == kingdomName) {
            return kingdomPositions[i];
        }
    }
    return {-1, -1}; // Return invalid position if kingdom not found
}

void MapSystem::saveMapToFile() const {
    ofstream saveFile("map_save.txt");
    if (saveFile.is_open()) {
        saveFile << kingdomCount << endl;
        for (int i = 0; i < kingdomCount; i++) {
            saveFile << kingdomNames[i] << endl;
            saveFile << kingdomPositions[i].x << endl;
            saveFile << kingdomPositions[i].y << endl;
        }
        saveFile.close();
    }
}

void MapSystem::loadMapFromFile() {
    ifstream loadFile("map_save.txt");
    if (loadFile.is_open()) {
        loadFile >> kingdomCount;
        for (int i = 0; i < kingdomCount; i++) {
            loadFile >> kingdomNames[i];
            loadFile >> kingdomPositions[i].x;
            loadFile >> kingdomPositions[i].y;
        }
        loadFile.close();
    }
} 