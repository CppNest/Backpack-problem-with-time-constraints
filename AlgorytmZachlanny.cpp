// Algorytm Zachłanny
// Algorytm Zachłanny wykorzystuje przygotowane instancje danych testowych zapisane w formacie JSON. Algorytm stosuje strategię wyboru lokalnie najlepszego rozwiązania na każdym etapie, aby znaleźć globalnie dobre (choć niekoniecznie optymalne) rozwiązanie. W tym przypadku algorytm sortuje przedmioty według stosunku wartości do wagi (value/weight) w porządku malejącym. Następnie iteracyjnie dodaje przedmioty do plecaka, o ile ich waga i czas pakowania nie przekroczą maksymalnych limitów. Dzięki temu algorytm priorytetowo wybiera przedmioty, które oferują najwyższą wartość w stosunku do kosztu (wagowego i czasowego), co prowadzi do efektywnego wykorzystania dostępnych zasobów.

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "libs/json.hpp"

struct Item {
    int id;
    int value;
    int weight;
    int packingTime;
};

bool compareByValuePerWeight(const Item& a, const Item& b) {
    return (double)a.value / a.weight > (double)b.value / b.weight;
}

void greedyAlgorithm(const std::string& fileName, int& totalValue, int& totalWeight, int& totalTime) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku " << fileName << "!\n";
        return;
    }

    nlohmann::json jsonData;
    file >> jsonData;
    file.close();

    int maxWeight = jsonData["knapsack"]["maxWeight"];
    int maxTime = jsonData["knapsack"]["maxTime"];

    std::vector<Item> items;
    for (const auto& jsonItem : jsonData["knapsack"]["items"]) {
        items.push_back({
            jsonItem["id"],
            jsonItem["value"],
            jsonItem["weight"],
            jsonItem["packingTime"]
        });
    }

    std::sort(items.begin(), items.end(), compareByValuePerWeight);

    totalValue = totalWeight = totalTime = 0;
    for (const auto& item : items) {
        if (totalWeight + item.weight <= maxWeight && totalTime + item.packingTime <= maxTime) {
            totalValue += item.value;
            totalWeight += item.weight;
            totalTime += item.packingTime;
        }
    }
}

int main() {
    int totalValue, totalWeight, totalTime;

    greedyAlgorithm("./TestData/TestData1.json", totalValue, totalWeight, totalTime);

    std::cout << "Zachłanne rozwiązanie:\n";
    std::cout << "Całkowita wartość: " << totalValue << "\n";
    std::cout << "Całkowita waga: " << totalWeight << "\n";
    std::cout << "Całkowity czas: " << totalTime << "\n";

    return 0;
}