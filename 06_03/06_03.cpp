#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <locale>
#include <cmath>

void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void merge_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    merge_sort(arr, left, mid);
    merge_sort(arr, mid + 1, right);

    merge(arr, left, mid, right);
}

void parallel_merge_sort(std::vector<int>& arr, int left, int right) {
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

#pragma omp task
    parallel_merge_sort(arr, left, mid);
#pragma omp task
    parallel_merge_sort(arr, mid + 1, right);

#pragma omp taskwait
    merge(arr, left, mid, right);
}

double calculate_pi(int n) {
    int count = 0;
#pragma omp parallel for reduction(+:count)
    for (int i = 0; i < n; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            count++;
        }
    }
    return 4.0 * count / n;
}

int main() {
    setlocale(LC_ALL, "Ru");
    //T2
    int size = 10000;
    std::vector<int> arr(size);

    for (int i = 0; i < size; i++)
        arr[i] = rand() % size;

    std::vector<int> arr_copy(arr);

    auto start_time = std::chrono::high_resolution_clock::now();

    merge_sort(arr_copy, 0, size - 1);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Время последовательной сортировки: " << duration << " миллисекунд" << std::endl;

    auto start_time_parallel = std::chrono::high_resolution_clock::now();

#pragma omp parallel
    {
#pragma omp single
        parallel_merge_sort(arr, 0, size - 1);
    }

    auto end_time_parallel = std::chrono::high_resolution_clock::now();
    auto duration_parallel = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_parallel - start_time_parallel).count();

    std::cout << "Время параллельной сортировки: " << duration_parallel << " миллисекунд" << std::endl;

    //T1
    int n, sum = 0;
    std::cout << "Введите размер массива: ";
    std::cin >> n;

    int* arrs = new int[n];

    srand(time(0));
    for (int i = 0; i < n; i++) {
        arrs[i] = rand() % 100;
    }

#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += arrs[i];
    }

    std::cout << "Сумма элементов массива: " << sum << std::endl;

    // Monte Carlo
    int num_points = 1000000;
    double pi = calculate_pi(num_points);
    std::cout << "Приближенное значение Pi: " << pi << std::endl;

    delete[] arrs;
    return 0;
}
