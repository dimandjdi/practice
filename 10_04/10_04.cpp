#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace cv;

int main() {

    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;
    CascadeClassifier smile_cascade;
    if (!face_cascade.load("haarcascade_frontalface_default.xml")) {
        cout << "Error loading face cascade" << endl;
        return -1;
    }
    if (!eyes_cascade.load("haarcascade_eye.xml")) {
        cout << "Error loading eyes cascade" << endl;
        return -1;
    }
    if (!smile_cascade.load("haarcascade_smile.xml")) {
        cout << "Error loading smile cascade" << endl;
        return -1;
    }

    VideoCapture cap("vid.mp4");
    if (!cap.isOpened()) {
        cout << "Error opening video file" << endl;
        return -1;
    }

    int fourcc = VideoWriter::fourcc('M', 'P', '4', 'V');
    int fps = cap.get(CAP_PROP_FPS);
    Size frame_size(static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH)),
        static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT)));
    VideoWriter out("output.mp4", fourcc, fps, frame_size);

    Mat frame, gray, blurred;

    auto start_time = chrono::high_resolution_clock::now();

    while (true) {
        int frame_counter = 0;
        cap >> frame;
        if (frame.empty()) {
            cout << "Error reading frame from video file" << endl;
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        GaussianBlur(gray, blurred, Size(7, 7), 0);

        vector<Rect> faces;
        face_cascade.detectMultiScale(blurred, faces, 1.1, 9);

#pragma omp parallel for
        for (int i = 0; i < faces.size(); i++) {
            const auto& face = faces[i];

            rectangle(frame, face, Scalar(255, 0, 0), 2);

            vector<Rect> eyes;
            eyes_cascade.detectMultiScale(blurred(face), eyes, 1.1, 35, CASCADE_SCALE_IMAGE, Size(30, 30));

            for (const auto& eye : eyes) {
                if (eye.area() > 0.01 * face.area()) {
                    rectangle(frame, eye + face.tl(), Scalar(0, 255, 0), 2);
                }
            }

            vector<Rect> smiles;
            smile_cascade.detectMultiScale(blurred(face), smiles, 2, 20, CASCADE_SCALE_IMAGE, Size(30, 30));

            for (const auto& smile : smiles) {
                if (smile.area() > 0.05 * face.area()) {
                    rectangle(frame, smile + face.tl(), Scalar(0, 0, 255), 2);
                }
            }
        }

        out << frame;

        imshow("Frame", frame);

        if (waitKey(1) == 27) {
            break;
        }
        frame_counter++;
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "Processing time: " << duration << " ms" << endl;

    cap.release();
    out.release();
    destroyAllWindows();

    return 0;
}
