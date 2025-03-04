// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Includy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <random>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <utility>
#include <omp.h>
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Generator~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BruteForce~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Greedy~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Meta~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Individual {
    std::vector<int> chromosome;
    int fitness;
};

const int POPULATION_SIZE = 50;
const int GENERATIONS = 100;
const double CROSSOVER_RATE = 0.7;
const double MUTATION_RATE = 0.1;

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

void evaluatePopulation(const std::vector<Item>& items, std::vector<Individual*>& population, int maxWeight, int maxTime) {
    #pragma omp parallel for
    for (size_t i = 0; i < population.size(); ++i) {
        population[i]->fitness = calculateFitness(items, population[i]->chromosome, maxWeight, maxTime);
    }
}

std::vector<Individual*> topIndividuals;
const int TOP_COUNT = 10;

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

void crossover(Individual* parent1, Individual* parent2) {
    static thread_local std::mt19937 gen(42 + omp_get_thread_num());
    std::uniform_int_distribution<> dist(0, parent1->chromosome.size() - 1);

    int crossoverPoint = dist(gen);
    for (size_t i = crossoverPoint; i < parent1->chromosome.size(); ++i) {
        std::swap(parent1->chromosome[i], parent2->chromosome[i]);
    }
}

void mutate(Individual* individual) {
    static thread_local std::mt19937 gen(42 + omp_get_thread_num());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    for (auto& gene : individual->chromosome) {
        if (dist(gen) < MUTATION_RATE) {
            gene = 1 - gene;
        }
    }
}

void GeneticAlgorithm(const std::vector<Item>& items, int maxWeight, int maxTime, int& totalValue, int& totalWeight, int& totalTime, std::vector<int>& selectedItems) {
    int n = items.size();
    std::vector<Individual*> currentPopulation, workingPopulation;
    initializePopulation(currentPopulation, n);

    static thread_local std::mt19937 gen(42);
    std::uniform_real_distribution<> realDist(0.0, 1.0);

    for (int generation = 0; generation < GENERATIONS; ++generation) {
        evaluatePopulation(items, currentPopulation, maxWeight, maxTime);
        workingPopulation.clear();

        #pragma omp parallel
        {
            std::vector<Individual*> localBuffer;
            #pragma omp for
            for (int i = 0; i < POPULATION_SIZE / 2; ++i) {
                auto [parent1, parent2] = tournamentSelection(currentPopulation);

                if (realDist(gen) < CROSSOVER_RATE) {
                    crossover(parent1, parent2);
                }

                mutate(parent1);
                mutate(parent2);

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
        currentPopulation = workingPopulation;
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

