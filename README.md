# Problem plecakowy z ograniczeniami czasowymi

## Opis projektu
Projekt zrealizowany na studia na przedmiot optymalizacja kombinatoryczna. Do każdego przedmiotu przypisywany jest wymagany czas zapakowania do plecaka. Łączny czas pakowania nie może przekroczyć ustalonego limitu. Rozwiązanie tego problemu polega na maksymalizacji wartości plecaka przy jednoczesnym uwzględnieniu ograniczenia czasowego.

## Cel projektu
Celem projektu jest stworzenie aplikacji, która pozwala na rozwiązanie problemu plecakowego z ograniczeniami czasowymi poprzez różne algorytmy, takie jak algorytm pełny, algorytm zachłanny oraz metahuerystyki.

## Jak uruchomić aplikację
1. Otwórz terminal w katalogu projektu.
2. Użyj kompilatora g++ do skompilowania plików źródłowych:
    ```sh
    g++ -o AlgorytmPelny AlgorytmPelny.cpp
    g++ -o Algorytmy Algorytmy.cpp
    g++ -o AlgorytmZachlanny AlgorytmZachlanny.cpp
    g++ -o AlgorytmZachlannyUlosowiony AlgorytmZachlannyUlosowiony.cpp
    g++ -o GeneratorDanych GeneratorDanych.cpp
    g++ -o Metahuerystyka Metahuerystyka.cpp
    g++ -o TestyCzasowe TestyCzasowe.cpp
    ```
3. Uruchom odpowiednie pliki wykonywalne:
    ```sh
    ./AlgorytmPelny
    ./Algorytmy
    ./AlgorytmZachlanny
    ./AlgorytmZachlannyUlosowiony
    ./GeneratorDanych
    ./Metahuerystyka
    ./TestyCzasowe
    ```

## Wymagania systemowe
- System operacyjny: Windows, macOS, Linux
- Kompilator C++ (np. g++)
- Terminal lub konsola

## Instrukcja instalacji
1. Upewnij się, że masz zainstalowany kompilator g++.
2. Sklonuj repozytorium projektu na swój lokalny komputer:
    ```sh
    git clone https://github.com/CppNest/Backpack-problem-with-time-constraints
    ```
3. Przejdź do katalogu projektu:
    ```sh
    cd Backpack-problem-with-time-constraints
    ```
4. Skompiluj pliki źródłowe:
    ```sh
    g++ -o AlgorytmPelny AlgorytmPelny.cpp
    g++ -o Algorytmy Algorytmy.cpp
    g++ -o AlgorytmZachlanny AlgorytmZachlanny.cpp
    g++ -o AlgorytmZachlannyUlosowiony AlgorytmZachlannyUlosowiony.cpp
    g++ -o GeneratorDanych GeneratorDanych.cpp
    g++ -o Metahuerystyka Metahuerystyka.cpp
    g++ -o TestyCzasowe TestyCzasowe.cpp
    ```

## Przykłady użycia
- Uruchomienie pełnego algorytmu:
    ```sh
    ./AlgorytmPelny
    ```
- Uruchomienie algorytmu zachłannego:
    ```sh
    ./AlgorytmZachlanny
    ```
- Uruchomienie metahuerystyki:
    ```sh
    ./Metahuerystyka
    ```

## License

This project is under the MIT License - see the [LICENSE](./LICENSE) file for details.
