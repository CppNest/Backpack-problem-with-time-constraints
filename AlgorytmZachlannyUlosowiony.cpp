// Algorytm Zachłanny Ulosowiony
// Algorytm Zachłanny Ulosowiony wykorzystuje Generator Danych Losowych, który najpierw wyświetla wylosowane dane, a następnie prezentuje wynik. Algorytm stosuje identyczną strategię jak Algorytm Zachłanny.

#include <iostream>
#include <vector>
#include <algorithm>
#include "GeneratorDanych.cpp"

bool compareByValuePerWeight(const Item& a, const Item& b) {
    return (double)a.value / a.weight > (double)b.value / b.weight;
}

void randomizedGreedyAlgorithm(const std::vector<Item>& items, int maxWeight, int maxTime, int& totalValue, int& totalWeight, int& totalTime) {
    totalValue = totalWeight = totalTime = 0;

    std::vector<Item> sortedItems = items;
    std::sort(sortedItems.begin(), sortedItems.end(), compareByValuePerWeight);

    for (const auto& item : sortedItems) {
        if (totalWeight + item.weight <= maxWeight && totalTime + item.packingTime <= maxTime) {
            totalValue += item.value;
            totalWeight += item.weight;
            totalTime += item.packingTime;
        }
    }
}

int main() {
    int n;
    int maxWeight, maxTime;

    std::cout << "Podaj liczbę przedmiotów: ";
    std::cin >> n;
    auto items = generateTestData(n, maxWeight, maxTime);

    std::cout << "Wygenerowane dane testowe:\n";
    printTestData(items, maxWeight, maxTime);

    int totalValue, totalWeight, totalTime;
    randomizedGreedyAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime);

    std::cout << "\nZachłanne rozwiązanie (na danych losowych):\n";
    std::cout << "Całkowita wartość: " << totalValue << "\n";
    std::cout << "Całkowita waga: " << totalWeight << "\n";
    std::cout << "Całkowity czas: " << totalTime << "\n";

    return 0;
}
