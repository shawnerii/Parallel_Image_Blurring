#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int NUM_THREADS = 8; // Adjust based on your CPU

// Flexible blur for a section of the image
void blur_section(const Mat& input, Mat& output, int start_row, int end_row, int kernel_size) {
    int width = input.cols;
    int height = input.rows;
    int k = kernel_size / 2; // Kernel radius

    for (int i = start_row; i <= end_row; ++i) {
        for (int j = k; j < width - k; ++j) {
            Vec3i sum = {0, 0, 0};

            for (int ki = -k; ki <= k; ++ki) {
                for (int kj = -k; kj <= k; ++kj) {
                    Vec3b pixel = input.at<Vec3b>(i + ki, j + kj);
                    sum[0] += pixel[0];
                    sum[1] += pixel[1];
                    sum[2] += pixel[2];
                }
            }

            int area = kernel_size * kernel_size;
            output.at<Vec3b>(i, j)[0] = static_cast<uchar>(sum[0] / area);
            output.at<Vec3b>(i, j)[1] = static_cast<uchar>(sum[1] / area);
            output.at<Vec3b>(i, j)[2] = static_cast<uchar>(sum[2] / area);
        }
    }
}

// Single-threaded flexible blur
void blur_single_threaded(const Mat& input, Mat& output, int kernel_size) {
    int width = input.cols;
    int height = input.rows;
    int k = kernel_size / 2;

    for (int i = k; i < height - k; ++i) {
        for (int j = k; j < width - k; ++j) {
            Vec3i sum = {0, 0, 0};

            for (int ki = -k; ki <= k; ++ki) {
                for (int kj = -k; kj <= k; ++kj) {
                    Vec3b pixel = input.at<Vec3b>(i + ki, j + kj);
                    sum[0] += pixel[0];
                    sum[1] += pixel[1];
                    sum[2] += pixel[2];
                }
            }

            int area = kernel_size * kernel_size;
            output.at<Vec3b>(i, j)[0] = static_cast<uchar>(sum[0] / area);
            output.at<Vec3b>(i, j)[1] = static_cast<uchar>(sum[1] / area);
            output.at<Vec3b>(i, j)[2] = static_cast<uchar>(sum[2] / area);
        }
    }
}

int main() {
    string filename;
    cout << "Enter path to the image file: ";
    getline(cin, filename);

    int kernel_size;
    cout << "Enter kernel size (odd number like 3, 5, 7, 9): ";
    cin >> kernel_size;

    if (kernel_size % 2 == 0 || kernel_size < 3) {
        cerr << "Error: Kernel size must be an odd number >= 3." << endl;
        return -1;
    }

    Mat input_image = imread(filename, IMREAD_COLOR);

    if (input_image.empty()) {
        cerr << "Error: Could not open or find the image!" << endl;
        return -1;
    }

    Mat output_image = input_image.clone(); // Single-threaded output

    // Single-threaded blur
    auto start_single = chrono::high_resolution_clock::now();
    blur_single_threaded(input_image, output_image, kernel_size);
    auto end_single = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_single = end_single - start_single;
    cout << "Single-threaded blur completed in " << duration_single.count() << " seconds." << endl;

    imwrite("blurred_color_single.jpg", output_image);

    // Prepare for multi-threaded blur
    output_image = input_image.clone();

    auto start_multi = chrono::high_resolution_clock::now();

    vector<thread> threads;
    int k = kernel_size / 2;
    int rows_per_thread = (input_image.rows - 2 * k) / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; ++t) {
        int start_row = k + t * rows_per_thread;
        int end_row = (t == NUM_THREADS - 1) ? (input_image.rows - k - 1) : (start_row + rows_per_thread - 1);

        threads.emplace_back(blur_section, cref(input_image), ref(output_image), start_row, end_row, kernel_size);
    }

    for (auto& th : threads) th.join();

    auto end_multi = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_multi = end_multi - start_multi;
    cout << "Multi-threaded blur completed in " << duration_multi.count() << " seconds." << endl;

    imwrite("blurred_color_multi.jpg", output_image);

    cout << "Blurred images saved as 'blurred_color_single.jpg' and 'blurred_color_multi.jpg'" << endl;

    return 0;
}