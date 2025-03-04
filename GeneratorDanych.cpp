// Generator danych testowych
// Generator liczb losowych w kodzie wykorzystuje funkcję randomInt, która zwraca liczby losowe z podanego zakresu. Kluczowym elementem jest ziarno, czyli wartość inicjalizująca generator – w tym przypadku stała 42, dzięki czemu wyniki są deterministyczne. Dzięki temu można łatwo testować kod, a dane będą powtarzalne.
// Funkcja printTestData wypisuje dane dotyczące plecaka, jego ograniczeń oraz listy przedmiotów w formacie JSON.

// Zmienne wykorzystane w kodzie:
// n - liczba przedmiotów w plecaku.
// maxWeight - maksymalna łączna waga, którą można zapakować. Jest to liczba losowa z przedziału 20 do 20 * n.
// maxTime - maksymalny czas dostępny na zapakowanie wszystkich przedmiotów. Jest to liczba losowa z przedziału 30 do 30 * n.
// item - obiekt struktury Item, reprezentujący pojedynczy przedmiot z atrybutami:
// id - identyfikator przedmiotu
// value - wartość przedmiotu
// weight - waga
// packingTime - czas potrzebny na spakowanie
// items - wektor przechowujący wszystkie wygenerowane przedmioty.

#include <iostream>
#include <vector>
#include <random>

struct Item {
    int id;
    int value;
    int weight;
    int packingTime;
};

int randomInt(int min, int max) {
    // static std::random_device rd;
    static std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

std::vector<Item> generateTestData(int n, int& maxWeight, int& maxTime) {
    std::vector<Item> items;

    maxWeight = randomInt(20, 20*n);
    maxTime = randomInt(30, 30*n);

    for (int i = 1; i <= n; ++i) {
        Item item;
        item.id = i;
        item.value = randomInt(10, 100);
        item.weight = randomInt(5, 30);
        item.packingTime = randomInt(5, 50);
        items.push_back(item);
    }

    return items;
}

void printTestData(const std::vector<Item>& items, int maxWeight, int maxTime) {
    std::cout << "{\n";
    std::cout << "  \"knapsack\": {\n";
    std::cout << "      \"maxWeight\": " << maxWeight << ",\n";
    std::cout << "      \"maxTime\": " << maxTime << ",\n";
    std::cout << "      \"items\": [\n";

    for (size_t i = 0; i < items.size(); ++i) {
        const auto& item = items[i];
        std::cout << "          { \"id\": " << item.id
                  << ", \"value\": " << item.value
                  << ", \"weight\": " << item.weight
                  << ", \"packingTime\": " << item.packingTime << " }";

        if (i < items.size() - 1) {
            std::cout << ",";
        }
        std::cout << "\n";
    }

    std::cout << "      ]\n";
    std::cout << "  }\n";
    std::cout << "}\n";
}

// int main() {
//     int n;
//     std::cout << "Podaj liczbę przedmiotów (n): ";
//     std::cin >> n;

//     int maxWeight, maxTime;
//     auto items = generateTestData(n, maxWeight, maxTime);

//     printTestData(items, maxWeight, maxTime);

//     return 0;
// }
