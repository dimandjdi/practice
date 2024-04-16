#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::Mat src = cv::imread("jap.jpg", cv::IMREAD_COLOR);

    if (src.empty())
    {
        std::cout << "Error" << std::endl;
        return -1;
    }

    cv::Mat hsv, lab, yuv, xyz, gray;

    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(src, lab, cv::COLOR_BGR2Lab);
    cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);
    cv::cvtColor(src, xyz, cv::COLOR_BGR2XYZ);
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    cv::imshow("HSV", hsv);
    cv::imshow("Lab", lab);
    cv::imshow("YUV", yuv);
    cv::imshow("XYZ", xyz);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 1.5, 1.5);

    cv::Mat edges;
    cv::Canny(gray, edges, 50, 150, 3);

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 200);

    for (size_t i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], line_ang = lines[i][1];
        cv::Point pt1, pt2;
        double a = cos(line_ang), b = sin(line_ang);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 800 * (-b));
        pt1.y = cvRound(y0 + 800 * (a));
        pt2.x = cvRound(x0 - 800 * (-b));
        pt2.y = cvRound(y0 - 800 * (a));
        cv::line(src, pt1, pt2, cv::Scalar(0, 0, 255), 3);
    }

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1, edges.rows / 8, 200, 100, 0, 0);

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        cv::circle(src, center, radius, cv::Scalar(0, 255, 0), 3, 8, 0);
    }

    cv::imshow("LineCircle", src);
    cv::waitKey(0);
    return 0;
}
