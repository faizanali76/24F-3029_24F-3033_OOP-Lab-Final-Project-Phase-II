#include "Stronghold.h"
#include <iostream>

using namespace std;

Kingdom::Kingdom() {
    kingdomName = "Unnamed Kingdom";
}

void Kingdom::setName(string name) {
    kingdomName = name;
}

string Kingdom::getName() const {
    return kingdomName;
} 