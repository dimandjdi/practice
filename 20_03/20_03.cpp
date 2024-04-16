#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>
#include <locale>
#include <windows.h>

int main() {

    setlocale(LC_ALL, "Ru");

    cv::Mat src = cv::imread("img.jpg", cv::IMREAD_COLOR);
    if (src.empty()) {
        std::cout << "Ошибка: Не удалось открыть или найти изображение" << std::endl;
        return -1;
    }

    cv::Mat gray, sepia, negative, edges;

#pragma omp sections
    {
#pragma omp section
        {
            cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        }

#pragma omp section
        {
            cv::Mat sepia_kernel = (cv::Mat_<double>(3, 3) << 0.272, 0.534, 0.131,
                0.349, 0.686, 0.168,
                0.393, 0.769, 0.189);
            cv::transform(src, sepia, sepia_kernel);
        }

#pragma omp section
        {
            cv::bitwise_not(src, negative);
        }

#pragma omp section
        {
            cv::Canny(src, edges, 100, 200);
        }
    }

    std::vector<std::pair<std::string, cv::Mat>> images = {
        {"Оригинал", src},
        {"Оттенки серого", gray},
        {"Сепия", sepia},
        {"Негатив", negative},
        {"Контур", edges}
    };

    int current_image = 0;

    cv::namedWindow("Изображение", cv::WINDOW_NORMAL);
    cv::setWindowProperty("Изображение", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::imshow("Изображение", images[current_image].second);

    while (true) {
        int key = cv::waitKey(0);
        if (key == 32) { // Пробел
            current_image = (current_image + 1) % images.size();
            cv::imshow("Изображение", images[current_image].second);
            std::cout << images[current_image].first << std::endl;
        }
        else if (key == 27 || key == 13) { // ESC или Enter
            break;
        }
    }

    cv::destroyAllWindows();

    return 0;
}
