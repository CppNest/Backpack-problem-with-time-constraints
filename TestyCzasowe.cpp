#include <iostream>
#include <vector>
#include <chrono>
#include "Algorytmy.cpp"  // Import wszystkich algorytmów


int main() {
    int n;

    std::cout << "Podaj liczbę przedmiotów: ";
    std::cin >> n;

    int maxWeight, maxTime;
    auto items = generateTestData(n, maxWeight, maxTime);

    int totalValue, totalWeight, totalTime;
    std::vector<int> selectedItems;

    // Czas dla GeneticAlgorithm
    auto start = std::chrono::high_resolution_clock::now();
    GeneticAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime, selectedItems);
    auto end = std::chrono::high_resolution_clock::now();
    auto durationGenetic = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout <<durationGenetic.count()<<"\n";

    // Resetowanie wyników
    totalValue = totalWeight = totalTime = 0;
    selectedItems.clear();

    // Czas dla bruteForceAlgorithm
    start = std::chrono::high_resolution_clock::now();
    bruteForceAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime, selectedItems);
    end = std::chrono::high_resolution_clock::now();
    auto durationBruteForce = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout <<durationBruteForce.count()<<"\n";

    // Resetowanie wyników
    totalValue = totalWeight = totalTime = 0;
    selectedItems.clear();

    // Czas dla randomizedGreedyAlgorithm
    start = std::chrono::high_resolution_clock::now();
    randomizedGreedyAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime);
    end = std::chrono::high_resolution_clock::now();
    auto durationGreedy = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout <<durationGreedy.count()<<"\n";

    return 0;
}
