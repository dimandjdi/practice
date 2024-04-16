#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>

const int Max_iter = 100;
const double min_x = -2.0;
const double max_x = 1.0;
const double min_y = -1.5;
const double max_y = 1.5;
const int max_color_ind = 50;
const int mid_color_ind = max_color_ind/2;

int mandelbrot(double real, double imag) {
    int iter = 0;
    std::complex<double> z(0, 0);
    std::complex<double> c(real, imag);

    while (iter < Max_iter && std::abs(z) <= 2.0) {
        z = z * z + c;
        iter++;
    }

    return iter;
}

cv::Vec3b color_map(int iter) {
    cv::Vec3b color;

    // Bounded color
    if (iter == Max_iter) {
        return color; 
    }

    int a = iter % max_color_ind;

    if (a > mid_color_ind) {
        a = max_color_ind - a;
    }


    int value = (a * 255) / max_color_ind;

    color[0] = value * 1.5; 
    color[1] = value / 2;
    color[2] = value / 4; 


    if (color[0] > 255) {
        color[0] = 255;
    }

    return color;
}



int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int width = 800;
    int height = 800;
    int chunk = height / size;
    int start_row = rank * chunk;
    int end_row = (rank == size - 1) ? height : start_row + chunk;

    cv::Mat image(height, width, CV_8UC3);
    cv::Mat local_image(end_row - start_row, width, CV_8UC3);

    for (int y = start_row; y < end_row; y++) {
        for (int x = 0; x < width; x++) {
            double real = min_x + (max_x - min_x) * x / width;
            double imag = min_y + (max_y - min_y) * y / height;

            int iter = mandelbrot(real, imag);
            cv::Vec3b color = color_map(iter);

            local_image.at<cv::Vec3b>(y - start_row, x)[0] = color[0]; 
            local_image.at<cv::Vec3b>(y - start_row, x)[1] = color[1]; 
            local_image.at<cv::Vec3b>(y - start_row, x)[2] = color[2]; 
        }
    }

    MPI_Gather(local_image.data, local_image.total() * local_image.channels(), MPI_UNSIGNED_CHAR,
        image.data, local_image.total() * local_image.channels(), MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD);

    if (rank == 0) {
        cv::namedWindow("Mandelbrot", cv::WINDOW_AUTOSIZE);
        cv::imshow("Mandelbrot", image);
        cv::waitKey(0);
        cv::destroyAllWindows();
        cv::imwrite("mandelbrot.png", image);
     

    }

    MPI_Finalize();
    return 0;
}
