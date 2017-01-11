cmake_minimum_required(VERSION 3.6)

# Use modern C++ for everything and generate compile_commands database.
set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED on)

project(envi-tools)

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(OpenCV REQUIRED)

add_executable(et_contrast main.cpp)
target_link_libraries(et_contrast
        Boost::filesystem
        opencv_core
        opencv_highgui
        opencv_imgproc
        )