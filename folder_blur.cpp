#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <filesystem>
#include <chrono>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;
using namespace std;
using namespace cv;

// Supported image extensions
const vector<string> valid_extensions = {".jpg", ".jpeg", ".png", ".bmp", ".tiff"};

// Mutex for printing and writing
mutex io_mutex;

// Create output folder if it doesn't exist
void create_output_folder(const string& folder) {
    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }
}

// Get all valid image paths from the input folder
vector<fs::path> get_image_files(const string& input_folder) {
    vector<fs::path> images;
    for (const auto& entry : fs::directory_iterator(input_folder)) {
        if (entry.is_regular_file()) {
            string ext = entry.path().extension().string();
            transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (find(valid_extensions.begin(), valid_extensions.end(), ext) != valid_extensions.end()) {
                images.push_back(entry.path());
            }
        }
    }
    return images;
}

// Apply blur to a single image and save it
void blur_image(const fs::path& input_path, const string& output_folder, int kernel_size) {
    Mat input = imread(input_path.string(), IMREAD_COLOR);
    if (input.empty()) {
        lock_guard<mutex> lock(io_mutex);
        cerr << "Failed to read: " << input_path << endl;
        return;
    }

    int k = kernel_size / 2;
    Mat output = input.clone();

    for (int i = k; i < input.rows - k; ++i) {
        for (int j = k; j < input.cols - k; ++j) {
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

    string output_path = output_folder + "/" + input_path.filename().string();
    imwrite(output_path, output);

    lock_guard<mutex> lock(io_mutex);
    cout << "Saved: " << output_path << endl;
}

// Single-threaded processing of all images
void blur_single_threaded_all(const vector<fs::path>& images, const string& output_folder, int kernel_size) {
    for (const auto& image_path : images) {
        blur_image(image_path, output_folder, kernel_size);
    }
}

// Worker thread for parallel blurring
void blur_worker(vector<fs::path>& images, mutex& queue_mutex, const string& output_folder, int kernel_size) {
    while (true) {
        fs::path image_path;
        {
            lock_guard<mutex> lock(queue_mutex);
            if (images.empty()) return;
            image_path = images.back();
            images.pop_back();
        }
        blur_image(image_path, output_folder, kernel_size);
    }
}

// Multi-threaded processing using task-level parallelism
void blur_multi_threaded_all(vector<fs::path> images, const string& output_folder, int kernel_size, int num_threads) {
    mutex queue_mutex;
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(blur_worker, ref(images), ref(queue_mutex), ref(output_folder), kernel_size);
    }
    for (auto& t : threads) t.join();
}


int main() {
    string input_folder = "Images";
    string output_folder_single = "Blurred_Single";
    string output_folder_multi = "Blurred_Multi";

    create_output_folder(output_folder_single);
    create_output_folder(output_folder_multi);

    vector<fs::path> image_files = get_image_files(input_folder);
    if (image_files.empty()) {
        cerr << "No images found in folder: " << input_folder << endl;
        return 1;
    }

    int kernel_size;
    cout << "Enter kernel size (odd number like 3, 5, 7): ";
    cin >> kernel_size;

    if (kernel_size % 2 == 0 || kernel_size < 3) {
        cerr << "Invalid kernel size. Must be an odd number >= 3." << endl;
        return 1;
    }

    int num_threads;
    cout << "Enter number of worker threads: ";
    cin >> num_threads;

    // Single-threaded pass
    auto start_single = chrono::high_resolution_clock::now();
    blur_single_threaded_all(image_files, output_folder_single, kernel_size);
    auto end_single = chrono::high_resolution_clock::now();
    double duration_single = chrono::duration<double>(end_single - start_single).count();
    cout << "\nSingle-threaded total time: " << duration_single << " seconds\n";

    // Multi-threaded pass
    auto start_multi = chrono::high_resolution_clock::now();
    blur_multi_threaded_all(image_files, output_folder_multi, kernel_size, num_threads);
    auto end_multi = chrono::high_resolution_clock::now();
    double duration_multi = chrono::duration<double>(end_multi - start_multi).count();
    cout << "Multi-threaded total time: " << duration_multi << " seconds\n";

    cout << "\nAll done. Check '" << output_folder_single << "' and '" << output_folder_multi << "' folders.\n";
    return 0;
}