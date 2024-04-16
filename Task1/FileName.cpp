#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>

std::mutex mtx;

void factorial_1thr(int n) {
    unsigned long long result = 1;
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 1; i <= n; i++) {
        result *= i;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    mtx.lock();
    std::cout << "Однопоточное вычисление: " << result << " за " << time.count() << " наносекунд." << std::endl;
    mtx.unlock();
}

void factorial_4thr(int n) {
    unsigned long long result = 1;
    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;

    int range = n / 4;

    for (int i = 0; i < 4; i++) {
        int part_start = range * i + 1;
        int part_end = (i == 3) ? n : range * (i + 1);
        threads.push_back(std::thread([&, part_start, part_end]() {
            unsigned long long partial_result = 1;
            for (int j = part_start; j <= part_end; j++) {
                partial_result *= j;
            }
            mtx.lock();
            result *= partial_result;
            mtx.unlock();
            }));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    mtx.lock();
    std::cout << "4-х поточное вычисление: " << result << " за " << time.count() << " наносекунд." << std::endl;
    mtx.unlock();
}

int main() {
    setlocale(LC_ALL, "Ru");

    int n;
    std::cout << "Введите число для вычисления факториала:  ";

    if (!(std::cin >> n) || n < 0) {
        std::cout << "Ошибка ввода. Введите целое неотрицательное число." << std::endl;
        return 0;
    }

    factorial_1thr(n);
    factorial_4thr(n);

    return 0;
}
