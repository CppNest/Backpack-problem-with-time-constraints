// Algorytm Pełnego Przeglądu Możliwych Rozwiązań
// Algorytm Pełnego Przeglądu Możliwych Rozwiązań wykorzystuje Generator Danych Losowych, który najpierw wyświetla wylosowane dane, a następnie prezentuje wynik.

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono> // Dodanie biblioteki do pomiaru czasu
#include "GeneratorDanych.cpp"

void bruteForceAlgorithm(const std::vector<Item>& items, int maxWeight, int maxTime, int& totalValue, int& totalWeight, int& totalTime, std::vector<int>& selectedItems) {
    totalValue = totalWeight = totalTime = 0;
    size_t n = items.size();
    int maxCombinationValue = 0;

    for (size_t combination = 0; combination < (1 << n); ++combination) {
        int currentWeight = 0, currentTime = 0, currentValue = 0;
        std::vector<int> currentSelected;

        for (size_t i = 0; i < n; ++i) {
            if (combination & (1 << i)) {
                currentWeight += items[i].weight;
                currentTime += items[i].packingTime;
                currentValue += items[i].value;
                currentSelected.push_back(items[i].id);
            }
        }

        if (currentWeight <= maxWeight && currentTime <= maxTime && currentValue > maxCombinationValue) {
            maxCombinationValue = currentValue;
            totalValue = currentValue;
            totalWeight = currentWeight;
            totalTime = currentTime;
            selectedItems = currentSelected;
        }
    }
}

int main() {
    int n;

    std::cout << "Podaj liczbę przedmiotów: ";
    std::cin >> n;

    int maxWeight, maxTime;
    auto items = generateTestData(n, maxWeight, maxTime);

    // std::cout << "Wygenerowane dane testowe:\n";
    // printTestData(items, maxWeight, maxTime);

    int totalValue, totalWeight, totalTime;
    std::vector<int> selectedItems;

    bruteForceAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime, selectedItems);

    // Wyświetlenie wyników dla tego powtórzenia
    std::cout << "\nNajlepsze rozwiązanie:\n";
    std::cout << "Całkowita wartość: " << totalValue << "\n";
    std::cout << "Całkowita waga: " << totalWeight << "\n";
    std::cout << "Całkowity czas: " << totalTime << "\n";
    std::cout << "Wybrane przedmioty (ID): ";
    for (int id : selectedItems) {
        std::cout << id << " ";
    }
    
    return 0;
}
