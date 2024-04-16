#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <vector>

bool isTriangle(const std::vector<cv::Point>& contour) {
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, cv::arcLength(contour, true) * 0.02, true);
    return approx.size() == 3;
}

void detect_edges(const std::string& image_path, const std::string& title, int blur_size, bool add_labels) {
    cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);

    if (img.empty()) {
        std::cout << "Error" << std::endl;
        return;
    }

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(blur_size, blur_size), 0);

    cv::Mat edges;
    cv::Canny(blurred, edges, 30, 90);

    cv::Mat colored_edges = cv::Mat::zeros(img.size(), CV_8UC3);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edges, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    cv::drawContours(colored_edges, contours, -1, cv::Scalar(255, 0, 0), 2);

    if (add_labels) {
        for (size_t i = 0; i < contours.size(); i++) {
            double area = cv::contourArea(contours[i]);
            cv::Rect bounding_rect = cv::boundingRect(contours[i]);
            float aspect_ratio = static_cast<float>(bounding_rect.width) / bounding_rect.height;

            std::string shape_label;
            if (isTriangle(contours[i])) {
                shape_label = "triangle";
            }
            else if (std::abs(1.0 - aspect_ratio) <= 0.05 && std::abs(1.0 - area / (bounding_rect.width * bounding_rect.height)) <= 0.05) {
                shape_label = "square";
            }
            else if (aspect_ratio >= 0.95 && aspect_ratio <= 1.05) {
                shape_label = "circle";
            }
            else {
                shape_label = "unknown";
            }

            cv::Point label_point(bounding_rect.x + bounding_rect.width / 2 - shape_label.size() * 4 / 2, bounding_rect.y + bounding_rect.height / 2);
            cv::putText(colored_edges, shape_label, label_point, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
        }
    }
    cv::namedWindow("Modified", cv::WINDOW_NORMAL);
    cv::imshow("Modified", colored_edges);
    cv::namedWindow("Origin", cv::WINDOW_NORMAL);
    cv::imshow("Origin", img);

    cv::waitKey(0);
    cv::destroyAllWindows();
}

int main() {
    detect_edges("figure.png", "figure", 3, true);
    detect_edges("text.jpg", "Text", 11, false);

    return 0;
}
