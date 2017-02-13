#include <fstream>
#include <iostream>
#include <random>
#include <boost/filesystem.hpp>
#include "envitools/CSVIO.hpp"

// returns random int on interval [a,b]
int RandomInt(int a, int b);

// argv == et_PointGenerator xmin, ymin, xmax, ymax, output path
int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << " Usage: " << argv[0];
        std::cout << " xmin, ymin, xmax, ymax, output path" << std::endl;
        return EXIT_FAILURE;
    }

    int num_points = 500;
    int xmin = std::stoi(argv[1]);
    int ymin = std::stoi(argv[2]);
    int xmax = std::stoi(argv[3]);
    int ymax = std::stoi(argv[4]);

    std::cout << "X,Y" << std::endl;
    std::vector<cv::Vec2i> vec;
    for (int i = 0; i < num_points; i++) {
        int x = RandomInt(xmin, xmax);
        int y = RandomInt(ymin, ymax);
        vec.emplace_back(x, y);
        std::cout << x << "," << y << std::endl;
    }

    boost::filesystem::path path = argv[5];

    if (boost::filesystem::exists(path)) {
        envitools::CSVIO::WriteCSV(vec, path);
    }

    return 0;
}

int RandomInt(int a, int b)
{
    std::random_device genDevice;
    std::uniform_int_distribution<int> genDist(a, b);
    return genDist(genDevice);
}
