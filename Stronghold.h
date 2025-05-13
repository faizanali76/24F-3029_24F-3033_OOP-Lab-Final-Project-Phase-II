#ifndef STRONGHOLD_H
#define STRONGHOLD_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Army;
class Population;
class Economy;
class Bank;
class ResourceManager;
class EventManager;
class Leader;

class Person {
protected:
    string personName;
    int personAge;
public:
    Person();
    Person(string n, int a);
    virtual void display() const;
};

// Simple class to store kingdom name
class Kingdom {
private:
    string kingdomName;
public:
    Kingdom();
    void setName(string name);
    string getName() const;
};

class SocialClass {
protected:
    string classTitle;
    int classSize;
public:
    SocialClass();
    SocialClass(string t, int pop);
    virtual void showStatus() const;
};

// Population class handles all citizen-related stuff
class Population {
private:
    int totalPopulation;
    int peasantCount;
    int merchantCount;
    int nobleCount;
    int foodReserves;
    float citizenHappiness;
public:
    Population();
    void simulate();
    void showStats() const;
    void saveToFile() const;
    void loadFromFile();
    int getTotal() const;
    void decrease(int amount);
    
    int getPeasantCount() const { return peasantCount; }
    int getMerchantCount() const { return merchantCount; }
    int getNobleCount() const { return nobleCount; }
    int getFoodReserves() const { return foodReserves; }
    float getHappiness() const { return citizenHappiness; }
    
    void setTotal(int value) { totalPopulation = value; }
    void setPeasantCount(int value) { peasantCount = value; }
    void setMerchantCount(int value) { merchantCount = value; }
    void setNobleCount(int value) { nobleCount = value; }
    void setFoodReserves(int value) { foodReserves = value; }
    void setHappiness(float value) { citizenHappiness = value; }
};

// Army class manages soldiers and their morale
class Army {
private:
    int soldierCount;
    int troopMorale;
    int militaryRations;
public:
    Army();
    void recruitAndTrain(Population& pop);
    void showStats() const;
    void saveToFile() const;
    void loadFromFile();
    void lowerMorale(int amount);
    
    int getSoldierCount() const { return soldierCount; }
    int getMorale() const { return troopMorale; }
    int getRations() const { return militaryRations; }
    
    void setSoldierCount(int value) { soldierCount = value; }
    void setMorale(int value) { troopMorale = value; }
    void setRations(int value) { militaryRations = value; }
};

// Economy class handles money and taxes
class Economy {
private:
    int stateTreasury;
    float currentTaxRate;
    float marketInflation;
public:
    Economy();
    void taxPopulation(const Population& pop);
    void spend(int amount);
    void showStats() const;
    void saveToFile() const;
    void loadFromFile();
    int getTreasury() const;
    void receiveLoan(int amount);
    
    float getTaxRate() const { return currentTaxRate; }
    float getInflation() const { return marketInflation; }
    
    void setTreasury(int value) { stateTreasury = value; }
    void setTaxRate(float value) { currentTaxRate = value; }
    void setInflation(float value) { marketInflation = value; }
};

// Bank class handles loans and fraud detection
class Bank {
private:
    int activeLoans;
    int detectedFraud;
public:
    Bank();
    void auditTreasury(Economy& economy);
    void issueLoan(Economy& economy, int amount);
    void repayLoan(Economy& economy, int amount);
    void showStats() const;
    void saveToFile() const;
    void loadFromFile();
    
    int getActiveLoans() const { return activeLoans; }
    int getDetectedFraud() const { return detectedFraud; }
    
    void setActiveLoans(int value) { activeLoans = value; }
    void setDetectedFraud(int value) { detectedFraud = value; }
};

// Manages all the resources in the game
class ResourceManager {
private:
    int foodStock;
    int timberStock;
    int stoneStock;
    int metalStock;
public:
    ResourceManager();
    void manage();
    void gatherResources();
    void consumeResources();
    void showStats() const;
    void saveToFile() const;
    void loadFromFile();
    void consumeFixed(string resourceType, int amount);
    
    int getFoodStock() const { return foodStock; }
    int getTimberStock() const { return timberStock; }
    int getStoneStock() const { return stoneStock; }
    int getMetalStock() const { return metalStock; }
    
    void setFoodStock(int value) { foodStock = value; }
    void setTimberStock(int value) { timberStock = value; }
    void setStoneStock(int value) { stoneStock = value; }
    void setMetalStock(int value) { metalStock = value; }
};

// Handles random events that can happen in the game
class EventManager {
public:
    EventManager();
    void trigger(Population& pop, Army& army, Economy& eco, ResourceManager& res);
    void famine(ResourceManager& res, Population& pop);
    void disease(Population& pop);
    void war(Army& army, Economy& eco);
    void betrayal(Economy& eco);
    void earthquake(ResourceManager& res);
};

// Base class for different types of rulers
class Leader {
protected:
    string rulerName;
public:
    Leader(string n);
    void imposePolicy(Economy& economy, Army& army);
    string getName() const;
};

// Good ruler who focuses on balance
class King : public Leader {
public:
    King();
    void imposePolicy_King(Economy& economy, Army& army);
};

// Bad ruler who's all about power
class Tyrant : public Leader {
public:
    Tyrant();
    void imposePolicy_Tyrant(Economy& economy, Army& army);
};

#endif 
