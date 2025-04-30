



# Parallel Image Blurring Project

## Summary

This project explores multithreaded programming in C++ using OpenCV to blur images efficiently. It demonstrates both **data-level parallelism** (dividing a single image across threads) and **task-level parallelism** (processing multiple images in parallel). The objective is to practice **multi-core programming techniques** and performance comparison using real-world image processing tasks.

---

## Requirements

- C++17 compatible compiler (`g++`, `clang++`, or MSVC)
- OpenCV installed
- Works on macOS, Linux, or Windows

---

## 1. Blurring a Single Image (`image_bluring.cpp`)

### Description

Blurs a single image using a convolutional kernel with multithreading. The image is divided into row segments, and each thread processes a portion of the image. This demonstrates **data-level parallelism** via **data decomposition**.

### How to Run (macOS/Linux)

**1. Compile:**
```bash
g++ image_bluring.cpp -o image_bluring `pkg-config --cflags --libs opencv4` -std=c++17

2. Run:

./image_bluring

You’ll be prompted to enter:
	•	Path to an image (e.g., cat.png)
	•	Kernel size (odd number like 3, 5, 7, 15)

 Output
	•	blurred_color_single.jpg – blurred using single thread
	•	blurred_color_multi.jpg – blurred using multiple threads

⸻

 2. Blurring Multiple Images (folder_blur.cpp)

 Description

Blurs all images inside the Images/ folder. Each image is processed in parallel using a different thread, showcasing task-level parallelism via task decomposition.

 How to Run (macOS/Linux)

1. Place images in the Images/ folder

2. Compile:

g++ folder_blur.cpp -o folder_blur `pkg-config --cflags --libs opencv4` -std=c++17

3. Run:

./folder_blur

You’ll be prompted to enter:
	•	Kernel size (odd number)
	•	Number of worker threads (e.g., 8)

 Output Folders
	•	Blurred_Single/ – outputs from single-threaded processing
	•	Blurred_Multi/ – outputs from multithreaded processing

⸻

 Parallelism Techniques Used

File	Parallelism Type	Decomposition Used
image_bluring.cpp	Data-level parallelism	Data decomposition (rows)
folder_blur.cpp	Task-level parallelism	Task decomposition (per image)



⸻

 Project Structure

Parallel_Image_Blurring/
├── image_bluring.cpp        # Single image multithreaded blurring
├── folder_blur.cpp          # Multiple image multithreaded blurring
├── Images/                  # Input images (ignored in Git)
├── Blurred_Single/          # Output from single-threaded
├── Blurred_Multi/           # Output from multithreaded
├── blurred_color_single.jpg
├── blurred_color_multi.jpg
├── README.md
├── Report.pdf
└── .gitignore

