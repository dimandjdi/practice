#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <thread>
#include <chrono>

const int size = 30;
const int ITERATIONS = 50;


void print_grid(bool grid[size][size]) {

    std::cout << "+";
    for (int i = 0; i < size * 2; i++) {
        std::cout << "-";
    }
    std::cout << "+" << std::endl;

    for (int i = 0; i < size; i++) {

        std::cout << "|";
        for (int j = 0; j < size; j++) {
            std::cout << (grid[i][j] ? '*' : ' ') << ' ';
        }
        std::cout << "|" << std::endl;
    }

    std::cout << "+";
    for (int i = 0; i < size * 2; i++) {
        std::cout << "-";
    }
    std::cout << "+" << std::endl << std::endl;
}


int count_neighbors(bool grid[size][size], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (!(i == 0 && j == 0) && grid[(x + i + size) % size][(y + j + size) % size]) {
                count++;
            }
        }
    }
    return count;
}

void game_of_life(bool grid[size][size]) {
    bool new_grid[size][size];

#pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int neighbors = count_neighbors(grid, i, j);
            if (grid[i][j]) {
                new_grid[i][j] = (neighbors == 2 || neighbors == 3);
            }
            else {
                new_grid[i][j] = (neighbors == 3);
            }
        }
    }

#pragma omp parallel for
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size;j++) {
            grid[i][j] = new_grid[i][j];
        }
    }
}

int main() {
    bool grid[size][size];
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i][j] = rand() % 2;
        }
    }

    for (int i = 0; i < ITERATIONS; i++) {
        std::cout << i << std::endl;
        print_grid(grid);
        game_of_life(grid);
    }

    return 0;
}
