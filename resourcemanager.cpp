#include "Stronghold.h"

ResourceManager::ResourceManager() {
    foodStock = 500;
    timberStock = 300;
    stoneStock = 200;
    metalStock = 100;
}

void ResourceManager::manage() {
    cout << "\n==================================================\n";
    cout << "                    RESOURCE MANAGEMENT           \n";
    cout << "==================================================\n";
    
    int foodGathered = 50;
    int timberGathered = 30;
    int stoneGathered = 20;
    int metalGathered = 10;
    
    foodStock += foodGathered;
    timberStock += timberGathered;
    stoneStock += stoneGathered;
    metalStock += metalGathered;
    
    cout << "Gathered Resources:\n";
    cout << "- Food: +" << foodGathered << " units\n";
    cout << "- Wood: +" << timberGathered << " units\n";
    cout << "- Stone: +" << stoneGathered << " units\n";
    cout << "- Metal: +" << metalGathered << " units\n";
    cout << "==================================================\n";
}

// Gathers new resources
void ResourceManager::gatherResources() {
    foodStock += 20;
    timberStock += 15;
    stoneStock += 10;
    metalStock += 5;
}

// Consumes resources for various needs
void ResourceManager::consumeResources() {
    foodStock -= 10;
    timberStock -= 5;
    stoneStock -= 3;
    metalStock -= 2;
}

// Shows current resource levels
void ResourceManager::showStats() const {
    cout << "\n==================================================\n";
    cout << "                    RESOURCE OVERVIEW             \n";
    cout << "==================================================\n";
    cout << "Food: " << foodStock << " units\n";
    cout << "Wood: " << timberStock << " units\n";
    cout << "Stone: " << stoneStock << " units\n";
    cout << "Metal: " << metalStock << " units\n";
    cout << "==================================================\n";
}

// Saves resource data to a file
void ResourceManager::saveToFile() const {
    ofstream out("resources.txt");
    if (!out) {
        cout << "Error: Failed to save resource records.\n";
        return;
    }
    
    out << foodStock << endl;
    out << timberStock << endl;
    out << stoneStock << endl;
    out << metalStock << endl;
    out.close();
    cout << "Resource records archived successfully.\n";
}

// Loads resource data from a file
void ResourceManager::loadFromFile() {
    ifstream in("resources.txt");
    if (!in) {
        cout << "Error: Failed to load resource records.\n";
        return;
    }
    
    in >> foodStock >> timberStock >> stoneStock >> metalStock;
    in.close();
    cout << "Resource records restored successfully.\n";
}

// Consumes a specific amount of a resource
void ResourceManager::consumeFixed(string resourceType, int amount) {
    if (resourceType == "food") {
        foodStock -= amount;
    } else if (resourceType == "wood") {
        timberStock -= amount;
    } else if (resourceType == "stone") {
        stoneStock -= amount;
    } else if (resourceType == "metal") {
        metalStock -= amount;
    }
}
