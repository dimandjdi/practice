#include <iostream>
#include <clocale>
#include <omp.h>
#include <limits>
using namespace std;
//T1
void print_hello(int n) {
    for (int i = 0; i < n; i++) {
        cout << "Привет, Мир!" << endl;
    }
}
//T2
void print_thread_order(int num_threads) {
#pragma omp parallel for
    {

#pragma omp barrier
        for (int i = 0; i < num_threads; i++) {
            std::cout << "Я поток " << i << " из " << num_threads << " потоков" << std::endl;
        }
    }
}


//T3
void print_thread_number(int num_threads) {
    omp_set_num_threads(num_threads);
#pragma omp parallel
    {
        cout << "Поток " << omp_get_thread_num() << " выполняется." << endl;
    }
}
//T4
void print_part_total_sum(int num, int num_threads) {
    omp_set_num_threads(num_threads);
    long long partial_sum = 0;
    long long total_sum = 0;

#pragma omp parallel shared(total_sum) private(partial_sum)
    {
#pragma omp for reduction(+:partial_sum)
        for (int i = 0; i <= num; i++) {
            partial_sum += i;
        }

#pragma omp critical
        {
            cout << "Частичная сумма: " << partial_sum << endl;
            total_sum += partial_sum;
        }
    }

    cout << "Общая сумма: " << total_sum << endl;
}

int main() {
    setlocale(LC_ALL, "Ru");

    int choice;
    int num, num_threads;

    do {
        cout << "\nВыберите опцию:\n";
        cout << "1. Вывести приветствия\n";
        cout << "2. Вывести порядок выполнения потоков\n";
        cout << "3. Вывести номера потоков\n";
        cout << "4. Вывести частичные и общую сумму\n";
        cout << "5. Выйти\n";
        cout << "Введите ваш выбор: ";
        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Неверный ввод. Пожалуйста, введите целое число: ";
        }

        switch (choice) {
        case 1:
            cout << "Введите количество приветствий: ";
            cin >> num;
            print_hello(num);
            break;
        case 2:
            cout << "Введите количество потоков: ";
            cin >> num_threads;
            print_thread_order(num_threads);
            break;
        case 3:
            cout << "Введите количество потоков: ";
            cin >> num_threads;
            print_thread_number(num_threads);
            break;
        case 4:
            cout << "Введите число: ";
            cin >> num;
            cout << "Введите количество потоков: ";
            cin >> num_threads;
            print_part_total_sum(num, num_threads);
            break;
        case 5:
            cout << "Выход...\n";
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова.\n";
            break;
        }
    } while (choice != 5);

    return 0;
}
