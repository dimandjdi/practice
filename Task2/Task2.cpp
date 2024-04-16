#include <iostream>
#include <thread>
#include <mutex>
#include <fstream>

std::mutex mtx;
int count = 0;

void Synch() {
    for (int i = 0; i < 10000; ++i) {

        mtx.lock();
        count++;

        mtx.unlock();
    }
}

void Not_Synch() {
    for (int i = 0; i < 10000; ++i) {
        count++;
    }
}

int main() {
    setlocale(LC_ALL, "Ru");
    // ��� ������������� ��������
    int count_nsynch = 0;
    count = 0;
    std::thread t1_nsynch(Not_Synch);
    std::thread t2_nsynch(Not_Synch);

    t1_nsynch.join();
    t2_nsynch.join();

    count_nsynch = count;

    std::cout << "Count ��� ������������� mutex: " << count_nsynch << std::endl;

    // � �������������� ��������
    int count_synch = 0;
    count = 0;
    std::thread t1(Synch);
    std::thread t2(Synch);

    t1.join();
    t2.join();

    count_synch = count;

    std::cout << "Count � �������������� mutex: " << count_synch << std::endl;

    // ������ � ����
    std::ofstream outputFile("output.txt", std::ios_base::app); // ������� ���� � ������ ��������
    if (outputFile.is_open()) {
        outputFile << "Count ��� ������������� mutex: " << count_nsynch << std::endl;
        outputFile << "Count � �������������� mutex: " << count_synch << std::endl;
        outputFile << "-----------------------------" << std::endl;
        outputFile.close();
    }
    else {
        std::cout << "�� ���������� ������� ����";
    }

    return 0;
}
