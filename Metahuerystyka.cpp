// Metaheurystyka (algorytm genetyczny)
// Algorytm Genetyczny wykorzystuje Generator Danych Losowych, a następnie prezentuje wynik. Populacja osobników, reprezentowanych przez chromosomy, jest iteracyjnie przetwarzana przez selekcję, krzyżowanie i mutację, aby znaleźć najlepsze rozwiązanie. Program dynamicznie ocenia przystosowanie każdego osobnika, uwzględniając wagę i czas, a następnie wybiera najlepsze rozwiązanie po zakończeniu generacji. Dzięki wykorzystaniu OpenMP algorytm działa wielowątkowo, co umożliwia efektywne wykorzystanie dostępnych zasobów obliczeniowych.

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <omp.h>
#include "GeneratorDanych.cpp"

struct Individual {
    std::vector<int> chromosome;
    int fitness;
};

const int POPULATION_SIZE = 50; // Rozmiar populacji
const int GENERATIONS = 100;    // Liczba generacji algorytmu
const double CROSSOVER_RATE = 0.7; // Prawdopodobieństwo krzyżowania
const double MUTATION_RATE = 0.1;  // Prawdopodobieństwo mutacji

// Funkcja oblicza przystosowanie (fitness) dla osobnika
int calculateFitness(const std::vector<Item>& items, const std::vector<int>& chromosome, int maxWeight, int maxTime) {
    int totalValue = 0, totalWeight = 0, totalTime = 0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (chromosome[i] == 1) {
            totalValue += items[i].value;
            totalWeight += items[i].weight;
            totalTime += items[i].packingTime;
        }
    }
    return (totalWeight <= maxWeight && totalTime <= maxTime) ? totalValue : 0;
}

// Funkcja inicjalizuje populację osobników
void initializePopulation(std::vector<Individual*>& population, int n) {
    static thread_local std::mt19937 gen(42);
    std::uniform_int_distribution<> dist(0, 1);
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        Individual* individual = new Individual;
        individual->chromosome.resize(n);
        for (int j = 0; j < n; ++j) {
            individual->chromosome[j] = dist(gen);
        }
        individual->fitness = 0;
        population.push_back(individual);
    }
}

// Funkcja ocenia całą populację i aktualizuje ich przystosowanie
void evaluatePopulation(const std::vector<Item>& items, std::vector<Individual*>& population, int maxWeight, int maxTime) {
    #pragma omp parallel for
    for (size_t i = 0; i < population.size(); ++i) {
        population[i]->fitness = calculateFitness(items, population[i]->chromosome, maxWeight, maxTime);
    }
}

std::vector<Individual*> topIndividuals; // Przechowuje najlepsze osobniki
const int TOP_COUNT = 10; // Liczba najlepszych osobników

// Funkcja dokonuje selekcji turniejowej i zwraca parę rodziców
std::pair<Individual*, Individual*> tournamentSelection(const std::vector<Individual*>& population) {
    static thread_local std::mt19937 gen(42 + omp_get_thread_num());
    std::uniform_int_distribution<> dist(0, POPULATION_SIZE - 1);

    auto selectBest = [&]() -> Individual* {
        Individual* best = population[dist(gen)];
        for (int i = 0; i < 2; ++i) {
            Individual* candidate = population[dist(gen)];
            if (candidate->fitness > best->fitness) {
                best = candidate;
            }
        }
        return best;
    };

    Individual* parent1 = selectBest();
    Individual* parent2 = selectBest();

    #pragma omp critical
    {
        for (Individual* parent : {parent1, parent2}) {
            if (topIndividuals.size() < TOP_COUNT || parent->fitness > topIndividuals.back()->fitness) {
                if (topIndividuals.size() == TOP_COUNT) {
                    topIndividuals.pop_back();
                }
                topIndividuals.insert(std::upper_bound(topIndividuals.begin(), topIndividuals.end(), parent, [](const Individual* a, const Individual* b) {
                    return a->fitness > b->fitness;
                }), new Individual(*parent));
            }
        }
    }

    return {parent1, parent2};
}

// Funkcja wykonuje krzyżowanie (crossover) między dwoma rodzicami
void crossover(Individual* parent1, Individual* parent2) {
    static thread_local std::mt19937 gen(42 + omp_get_thread_num());
    std::uniform_int_distribution<> dist(0, parent1->chromosome.size() - 1);

    int crossoverPoint = dist(gen);
    for (size_t i = crossoverPoint; i < parent1->chromosome.size(); ++i) {
        std::swap(parent1->chromosome[i], parent2->chromosome[i]);
    }
}

// Funkcja wykonuje mutację na danym osobniku
void mutate(Individual* individual) {
    static thread_local std::mt19937 gen(42 + omp_get_thread_num());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    for (auto& gene : individual->chromosome) {
        if (dist(gen) < MUTATION_RATE) {
            gene = 1 - gene;
        }
    }
}

// Główna funkcja algorytmu genetycznego
void GeneticAlgorithm(const std::vector<Item>& items, int maxWeight, int maxTime, int& totalValue, int& totalWeight, int& totalTime, std::vector<int>& selectedItems) {
    int n = items.size();
    std::vector<Individual*> currentPopulation, workingPopulation;
    initializePopulation(currentPopulation, n);

    static thread_local std::mt19937 gen(42);
    std::uniform_real_distribution<> realDist(0.0, 1.0);

    for (int generation = 0; generation < GENERATIONS; ++generation) {
        evaluatePopulation(items, currentPopulation, maxWeight, maxTime); // Ocena przystosowania populacji
        workingPopulation.clear();

        #pragma omp parallel
        {
            std::vector<Individual*> localBuffer;
            #pragma omp for
            for (int i = 0; i < POPULATION_SIZE / 2; ++i) {
                auto [parent1, parent2] = tournamentSelection(currentPopulation); // Selekcja rodziców

                if (realDist(gen) < CROSSOVER_RATE) {
                    crossover(parent1, parent2); // Krzyżowanie rodziców
                }

                mutate(parent1); // Mutacja rodzica 1
                mutate(parent2); // Mutacja rodzica 2

                parent1->fitness = calculateFitness(items, parent1->chromosome, maxWeight, maxTime);
                parent2->fitness = calculateFitness(items, parent2->chromosome, maxWeight, maxTime);

                localBuffer.push_back(new Individual(*parent1));
                localBuffer.push_back(new Individual(*parent2));
            }
            #pragma omp critical
            workingPopulation.insert(workingPopulation.end(), localBuffer.begin(), localBuffer.end());
        }

        std::sort(workingPopulation.begin(), workingPopulation.end(), [](Individual* a, Individual* b) {
            return a->fitness > b->fitness;
        });

        for (size_t i = 0; i < std::min(topIndividuals.size(), workingPopulation.size()); ++i) {
            if (topIndividuals[i]->fitness > workingPopulation.back()->fitness) {
                std::swap(workingPopulation.back(), topIndividuals[i]);
                std::push_heap(workingPopulation.begin(), workingPopulation.end(), [](Individual* a, Individual* b) {
                    return a->fitness < b->fitness;
                });
                workingPopulation.pop_back();
            }
        }
        currentPopulation = workingPopulation; // Aktualizacja populacji
    }

    evaluatePopulation(items, currentPopulation, maxWeight, maxTime);
    Individual* bestIndividual = *std::max_element(
        currentPopulation.begin(), currentPopulation.end(),
        [](Individual* a, Individual* b) { return a->fitness < b->fitness; });

    totalValue = bestIndividual->fitness;
    totalWeight = totalTime = 0;
    selectedItems.clear();

    for (size_t i = 0; i < items.size(); ++i) {
        if (bestIndividual->chromosome[i] == 1) {
            totalWeight += items[i].weight;
            totalTime += items[i].packingTime;
            selectedItems.push_back(i);
        }
    }

    for (auto individual : currentPopulation) delete individual;
    for (auto individual : topIndividuals) delete individual;
}

int main() {
    int n;

    std::cout << "Podaj liczbę przedmiotów: ";
    std::cin >> n;

    int maxWeight, maxTime;
    auto items = generateTestData(n, maxWeight, maxTime);

    int totalValue, totalWeight, totalTime;
    std::vector<int> selectedItems;

    GeneticAlgorithm(items, maxWeight, maxTime, totalValue, totalWeight, totalTime, selectedItems);

    std::cout << "\nNajlepsze rozwiazanie:\n";
    std::cout << "Całkowita wartość: " << totalValue << "\n";
    std::cout << "Całkowita waga: " << totalWeight << "\n";
    std::cout << "Całkowity czas: " << totalTime << "\n";
    std::cout << "Wybrane przedmioty (ID): ";
    for (int id : selectedItems) {
        std::cout << id << " ";
    }

    return 0;
}
