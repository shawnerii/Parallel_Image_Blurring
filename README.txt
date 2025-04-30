===============================
Parallel Image Blurring Project
===============================

Requirements:
-------------
- C++17 compatible compiler (g++ or MSVC)
- OpenCV installed
- macOS, Linux, or Windows

---------------------------------------------------
1. Blurring a Single Image (image_bluring.cpp)
---------------------------------------------------

Description:
Applies blur to a single image using multithreading (data-level parallelism).

How to Run (macOS/Linux):

1. Compile:
   g++ image_bluring.cpp -o image_bluring `pkg-config --cflags --libs opencv4` -std=c++17

2. Run:
   ./image_bluring

Outputs:
- blurred_color_single.jpg
- blurred_color_multi.jpg

---------------------------------------------------
2. Blurring Multiple Images (folder_blur.cpp)
---------------------------------------------------

Description:
Blurs all images in the "Images/" folder using task-level parallelism.

How to Run (macOS/Linux):

1. Place images in the "Images/" folder.

2. Compile:
   g++ folder_blur.cpp -o folder_blur `pkg-config --cflags --libs opencv4` -std=c++17

3. Run:
   ./folder_blur

Outputs:
- Blurred_Single/  → Sequential results
- Blurred_Multi/   → Parallel results

---------------------------------------------------
Windows Note:
---------------------------------------------------
Project is compatible with Windows (MSVC or MinGW).  
Use OpenCV for Windows and configure include/lib paths manually.  
Ensure C++17 is enabled during compilation.