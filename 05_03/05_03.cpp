#include <opencv2/opencv.hpp>

int main() {
    cv::Mat img = cv::imread("tiger.jpg", cv::IMREAD_COLOR);

    if (img.empty()) {
        std::cout << "Error" << std::endl;
        return -1;
    }

    cv::rectangle(img, cv::Point(50, 50), cv::Point(150, 150), cv::Scalar(0, 0, 255), 2);
    cv::circle(img, cv::Point(300, 300), 50, cv::Scalar(0, 255, 0), 2);
    cv::ellipse(img, cv::Point(350, 350), cv::Size(50, 100), 0, 0, 360, cv::Scalar(255, 0, 255), 2);
    cv::line(img, cv::Point(10, 200), cv::Point(1000, 200), cv::Scalar(255, 255, 0), 2);
    std::vector<cv::Point> triangle_pts;
    triangle_pts.push_back(cv::Point(500, 300)); 
    triangle_pts.push_back(cv::Point(600, 300)); 
    triangle_pts.push_back(cv::Point(550, 400)); 
    cv::polylines(img, std::vector<std::vector<cv::Point>>{triangle_pts}, true, cv::Scalar(0, 255, 255), 2);

    cv::namedWindow("Tiger", cv::WINDOW_NORMAL);
    cv::imshow("Tiger", img);

    int rows = img.rows;
    int cols = img.cols;

    cv::Mat part1 = img(cv::Rect(0, 0, cols / 2, rows / 2));
    cv::Mat part2 = img(cv::Rect(cols / 2, 0, cols / 2, rows / 2));
    cv::Mat part3 = img(cv::Rect(0, rows / 2, cols / 2, rows / 2));
    cv::Mat part4 = img(cv::Rect(cols / 2, rows / 2, cols / 2, rows / 2));

    cv::namedWindow("Part 1", cv::WINDOW_NORMAL);
    cv::imshow("Part 1", part1);
    cv::namedWindow("Part 2", cv::WINDOW_NORMAL);
    cv::imshow("Part 2", part2);
    cv::namedWindow("Part 3", cv::WINDOW_NORMAL);
    cv::imshow("Part 3", part3);
    cv::namedWindow("Part 4", cv::WINDOW_NORMAL);
    cv::imshow("Part 4", part4);

    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::circle(mask, cv::Point(img.cols / 2, img.rows / 2), 200, cv::Scalar(255), -1);

    cv::Mat result;
    img.copyTo(result, mask);

    cv::namedWindow("Result", cv::WINDOW_NORMAL);
    cv::imshow("Result", result);

    cv::waitKey(0);

    return 0;
}
