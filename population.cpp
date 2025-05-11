#include "Stronghold.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

// Start with a small population and decent happiness
Population::Population()
{
    totalPopulation = 100;
    peasantCount = 60;
    merchantCount = 25;
    nobleCount = 15;
    citizenHappiness = 70.0;
    foodReserves = 300.0;
}

// Simulates population changes like births, deaths, and class balance
void Population::simulate()
{
    cout << "\n==================================================\n";
    cout << "                    POPULATION MANAGEMENT          \n";
    cout << "==================================================\n";
    
    cout << "Current Population: " << totalPopulation << " citizens\n";
    cout << "Current Happiness: " << citizenHappiness << "%\n";
    cout << "Food Reserves: " << foodReserves << " units\n";
    
    cout << "\n1. Distribute Food\n";
    cout << "2. Manage Population Growth\n";
    cout << "3. Check Class Balance\n";
    cout << "4. Return to Main Menu\n";
    cout << "Choose an option: ";
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: {
            cout << "\nHow much food would you like to distribute? (0-" << foodReserves << "): ";
            float foodToDistribute;
            cin >> foodToDistribute;
            
            if (foodToDistribute > 0 && foodToDistribute <= foodReserves) {
                foodReserves -= foodToDistribute;
                citizenHappiness += (foodToDistribute / totalPopulation) * 2;
                if (citizenHappiness > 100) citizenHappiness = 100;
                cout << "Food distributed! Happiness increased.\n";
            } else {
                cout << "Invalid amount of food!\n";
            }
            break;
        }
        case 2: {
            cout << "\nCurrent Growth Rate: 10 citizens per turn\n";
            cout << "Would you like to adjust growth rate? (y/n): ";
            char adjust;
            cin >> adjust;
            
            if (adjust == 'y' || adjust == 'Y') {
                cout << "Enter new growth rate (-10 to 20): ";
                int newGrowth;
                cin >> newGrowth;
                
                if (newGrowth >= -10 && newGrowth <= 20) {
                    totalPopulation += newGrowth;
                    if (totalPopulation < 0) totalPopulation = 0;
                    cout << "Population growth adjusted.\n";
                } else {
                    cout << "Invalid growth rate!\n";
                }
            }
            break;
        }
        case 3: {
            float peasantRatio = (float)peasantCount / totalPopulation;
            float merchantRatio = (float)merchantCount / totalPopulation;
            float nobleRatio = (float)nobleCount / totalPopulation;
            
            cout << "\nCurrent Class Distribution:\n";
            cout << "- Peasants: " << (peasantRatio * 100) << "%\n";
            cout << "- Merchants: " << (merchantRatio * 100) << "%\n";
            cout << "- Nobles: " << (nobleRatio * 100) << "%\n";
            
            if (peasantRatio > 0.7) {
                cout << "Warning: Too many peasants! Social unrest may occur.\n";
                citizenHappiness -= 5;
            }
            if (merchantRatio < 0.1) {
                cout << "Warning: Too few merchants! Economic growth may suffer.\n";
                citizenHappiness -= 3;
            }
            if (nobleRatio > 0.3) {
                cout << "Warning: Too many nobles! Tax burden may increase.\n";
                citizenHappiness -= 4;
            }
            break;
        }
        case 4:
            return;
        default:
            cout << "Invalid choice!\n";
    }
    
    // Update class distribution
    peasantCount = totalPopulation * 0.6;
    merchantCount = totalPopulation * 0.25;
    nobleCount = totalPopulation * 0.15;
    
    if (citizenHappiness < 30) {
        cout << "Alert: Civil unrest detected!\n";
        int unrestCasualties = rand() % 10;
        totalPopulation -= unrestCasualties;
        cout << "Casualties from unrest: " << unrestCasualties << " citizens\n";
    }
    
    cout << "==================================================\n";
}

// Shows current population stats
void Population::showStats() const
{
    cout << "\n==================================================\n";
    cout << "                    POPULATION STATISTICS          \n";
    cout << "==================================================\n";
    cout << "Total Citizens: " << totalPopulation << "\n";
    cout << "Peasant Class: " << peasantCount << "\n";
    cout << "Merchant Class: " << merchantCount << "\n";
    cout << "Noble Class: " << nobleCount << "\n";
    cout << "Happiness Index: " << citizenHappiness << "%\n";
    cout << "Food Reserves: " << foodReserves << " units\n";
    cout << "==================================================\n";
}

// Saves population data to a file
void Population::saveToFile() const
{
    ofstream out("population.txt");
    if (!out)
    {
        cout << "\nError: Unable to archive population data.\n";
        return;
    }

    out << totalPopulation << endl;
    out << peasantCount << endl;
    out << merchantCount << endl;
    out << nobleCount << endl;
    out << citizenHappiness << endl;
    out << foodReserves << endl;
    out.close();
    cout << "\nPopulation data successfully archived.\n";
}

// Loads population data from a file
void Population::loadFromFile()
{
    ifstream in("population.txt");
    if (!in)
    {
        cout << "\nError: Unable to restore population data.\n";
        return;
    }

    in >> totalPopulation >> peasantCount >> merchantCount >> nobleCount >> citizenHappiness >> foodReserves;
    in.close();
    cout << "\nPopulation data successfully restored.\n";
}

// Returns total population count
int Population::getTotal() const
{
    return totalPopulation;
}

// Decreases population by given amount and updates class distribution
void Population::decrease(int amount)
{
    totalPopulation -= amount;
    if (totalPopulation < 0)
    {
        totalPopulation = 0;
    }

    peasantCount = totalPopulation * 0.6;
    merchantCount = totalPopulation * 0.25;
    nobleCount = totalPopulation * 0.15;
}
