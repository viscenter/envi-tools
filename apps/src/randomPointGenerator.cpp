#include <fstream>
#include <iostream>
#include <random>

#include <boost/filesystem.hpp>

#include "envitools/CSVIO.hpp"
#include "envitools/EnviUtils.hpp"

// argv == et_PointGenerator xmin, ymin, xmax, ymax, total points, output path
int main(int argc, char** argv) {
    if (argc < 7) {
        std::cout << " Usage: " << argv[0];
        std::cout << " [xmin] [ymin] [xmax] [ymax] [total points] [output path]"
                  << std::endl;
        return EXIT_FAILURE;
    }

    int xmin = std::stoi(argv[1]);
    int ymin = std::stoi(argv[2]);
    int xmax = std::stoi(argv[3]);
    int ymax = std::stoi(argv[4]);
    int num_points = std::stoi(argv[5]);

    std::cout << "X,Y" << std::endl;
    std::vector<cv::Vec2i> vec;
    for (int i = 0; i < num_points; i++) {
        int x = envitools::RandomInt(xmin, xmax);
        int y = envitools::RandomInt(ymin, ymax);
        vec.emplace_back(x, y);
        std::cout << x << "," << y << std::endl;
    }

    boost::filesystem::path path = argv[5];

    if (!(boost::filesystem::exists(path))) {
        envitools::CSVIO::WriteCSV(path, vec);
    }

    return 0;
}
