#include "Stronghold.h"

EventManager::EventManager() {
}

void EventManager::trigger(Population& pop, Army& army, Economy& eco, ResourceManager& res) {
    cout << "\n==================================================\n";
    cout << "                    EVENT TRIGGER MENU                     \n";
    cout << "==================================================\n";
    cout << "1. Agricultural Crisis                                  \n";
    cout << "2. Epidemic Outbreak                                   \n";
    cout << "3. Military Conflict                                   \n";
    cout << "4. Aristocratic Treason                                \n";
    cout << "5. Geological Disaster                                 \n";
    cout << "==================================================\n";
    cout << "Select event to trigger: ";
    int eventSelection;
    cin >> eventSelection;

    if (eventSelection == 1) {
        famine(res, pop);
    } else if (eventSelection == 2) {
        disease(pop);
    } else if (eventSelection == 3) {
        war(army, eco);
    } else if (eventSelection == 4) {
        betrayal(eco);
    } else if (eventSelection == 5) {
        earthquake(res);
    } else {
        cout << "Invalid selection.\n";
    }
}

// Bad harvest leads to food shortage and population problems
void EventManager::famine(ResourceManager& res, Population& pop) {
    cout << "\n==================================================\n";
    cout << "                    AGRICULTURAL CRISIS                    \n";
    cout << "==================================================\n";
    cout << "A devastating famine has struck the realm!\n";
    cout << "Food reserves depleted by 100 units.\n";
    cout << "Population suffers significant losses.\n";
    cout << "==================================================\n";
    res.consumeFixed("food", 100);
    pop.decrease(10);
}

// Disease spreads through the population
void EventManager::disease(Population& pop) {
    cout << "\n==================================================\n";
    cout << "                    EPIDEMIC OUTBREAK                      \n";
    cout << "==================================================\n";
    cout << "A deadly plague spreads through the population!\n";
    cout << "15 citizens have perished from the disease.\n";
    cout << "==================================================\n";
    pop.decrease(15);
}

// War breaks out, it will affect army and economy
void EventManager::war(Army& army, Economy& eco) {
    cout << "\n==================================================\n";
    cout << "                    MILITARY CONFLICT                      \n";
    cout << "==================================================\n";
    cout << "War has erupted on our borders!\n";
    cout << "Military morale has suffered a significant blow.\n";
    cout << "Treasury funds depleted for war efforts.\n";
    cout << "==================================================\n";
    army.lowerMorale(20);
    eco.spend(200);
}

// Nobles steal from the treasury
void EventManager::betrayal(Economy& eco) {
    cout << "\n==================================================\n";
    cout << "                    ARISTOCRATIC TREASON                   \n";
    cout << "==================================================\n";
    cout << "The noble houses have betrayed the realm!\n";
    cout << "300 gold has been stolen from the treasury.\n";
    cout << "==================================================\n";
    eco.spend(300);
}

// Earthquake damages stone resources
void EventManager::earthquake(ResourceManager& res) {
    cout << "\n==================================================\n";
    cout << "                    GEOLOGICAL DISASTER                    \n";
    cout << "==================================================\n";
    cout << "A powerful earthquake has shaken the kingdom!\n";
    cout << "Stone reserves have been severely damaged.\n";
    cout << "==================================================\n";
    res.consumeFixed("stone", 50);
}
