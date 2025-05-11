#include "Stronghold.h"

// Base constructor, just sets the name
Leader::Leader(string n) {
    rulerName = n;
}

string Leader::getName() const {
    return rulerName;
}

// Default policy that does nothing, kinda like a placeholder
void Leader::imposePolicy(Economy& economy, Army& army) {
    cout << rulerName << " has not implemented any specific policies.\n";
}

// King constructor, just calls the base constructor with "King"
King::King() : Leader("King") {}

// King's policies are all about balance and keeping people happy
void King::imposePolicy_King(Economy& economy, Army& army) {
    cout << "\n==================================================\n";
    cout << "                    KING'S POLICIES               \n";
    cout << "==================================================\n";
    cout << "The King has decreed new policies:\n";
    cout << "- Moderate tax rates for economic growth\n";
    cout << "- Balanced military spending\n";
    cout << "- Focus on population welfare\n";
    
    economy.setTaxRate(0.15);
    army.setMorale(army.getMorale() + 5);
}

// Tyrant constructor, just calls the base constructor with "Tyrant"
Tyrant::Tyrant() : Leader("Tyrant") {}

// Tyrant's policies are all about power and control
void Tyrant::imposePolicy_Tyrant(Economy& economy, Army& army) {
    cout << "\n==================================================\n";
    cout << "                    TYRANT'S POLICIES             \n";
    cout << "==================================================\n";
    cout << "The Tyrant has imposed harsh policies:\n";
    cout << "- High tax rates to fund military\n";
    cout << "- Increased military spending\n";
    cout << "- Reduced population welfare\n";
    
    economy.setTaxRate(0.30);
    army.setMorale(army.getMorale() - 10);
}
