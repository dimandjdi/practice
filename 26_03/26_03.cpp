#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap("vid.mp4");
    if (!cap.isOpened()) {
        std::cout << "Error" << std::endl;
        return -1;
    }

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::namedWindow("Video", cv::WINDOW_NORMAL);
    cv::resizeWindow("Video", frame_width, frame_height);

    cv::namedWindow("Mask", cv::WINDOW_NORMAL);
    cv::resizeWindow("Mask", frame_width, frame_height);

    cv::Mat frame, frame_HSV, frame_threshold, contours_img;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::Scalar lower_bound(70, 10, 130);
    cv::Scalar upper_bound(255, 50, 255);

    int fourcc = cv::VideoWriter::fourcc('H', '2', '6', '4'); 
    cv::VideoWriter out("output.mp4", fourcc, cap.get(cv::CAP_PROP_FPS), cv::Size(frame_width, frame_height), false);

    if (!out.isOpened()) {
        std::cout << "Error: Could not open the output video file for write." << std::endl;
        return -1;
    }

    while (true) {

        cap >> frame;
        if (frame.empty()) {
            break;
        }

        cv::cvtColor(frame, frame_HSV, cv::COLOR_BGR2HSV);
        cv::inRange(frame_HSV, lower_bound, upper_bound, frame_threshold);
        cv::erode(frame_threshold, frame_threshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
        cv::dilate(frame_threshold, frame_threshold, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
        cv::findContours(frame_threshold, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (size_t i = 0; i < contours.size(); i++) {

            std::vector<cv::Point> approx;
            cv::approxPolyDP(contours[i], approx, cv::arcLength(contours[i], true) * 0.02, true);

            cv::Moments contour_moments = cv::moments(contours[i]);
            int cx = contour_moments.m10 / contour_moments.m00;
            int cy = contour_moments.m01 / contour_moments.m00;

            std::string shape_name;
            if (approx.size() == 3) {
                shape_name = "Triangle";
            }
            else if (approx.size() == 4) {
                float aspect_ratio = (float)cv::boundingRect(approx).width / cv::boundingRect(approx).height;
                if (aspect_ratio >= 0.95 && aspect_ratio <= 1.05) {
                    shape_name = "Square";
                }
                else {
                    shape_name = "Rectangle";
                }
            }
            else {
                shape_name = "Unknown shape";
            }

            cv::putText(frame, shape_name, cv::Point(cx, cy), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
            cv::drawContours(frame, contours, i, cv::Scalar(0, 255, 0), 2);
        }

        out.write(frame);

        cv::imshow("Video", frame);
        cv::imshow("Mask", frame_threshold);

        //Esc
        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    cap.release();
    out.release();

    cv::destroyWindow("Video");
    cv::destroyWindow("Mask");

    return 0;
}
