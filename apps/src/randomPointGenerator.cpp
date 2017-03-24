#include <fstream>
#include <iostream>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "envitools/CSVIO.hpp"
#include "envitools/EnviUtils.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

// argv == et_PointGenerator xmin, ymin, xmax, ymax, total points, output path
int main(int argc, char** argv) {
    fs::path outputPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()("help,h", "Show this message")(
        "input-file,i", po::value<std::string>()->required(),
        "Input file path of ROIs");
    ("output-file,o", po::value<std::string>()->required(), "Output file path");
    ("number-points,p", po::value<int>()->required(),
     "total number of points to generate");
    // clang-format on

    // parsedOptions will hold the value of all parsed options as a Map
    po::variables_map parsedOptions;
    po::store(
        po::command_line_parser(argc, argv).options(options).run(),
        parsedOptions);

    // show the help message
    if (parsedOptions.count("help") || argc < 3) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    // warn of missing options
    try {
        po::notify(parsedOptions);
    } catch (po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
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
        auto x = envitools::RandomInt(xmin, xmax);
        auto y = envitools::RandomInt(ymin, ymax);
        vec.emplace_back(x, y);
        std::cout << x << "," << y << std::endl;
    }

    fs::path path = argv[5];

    if (!(boost::filesystem::exists(path))) {
        envitools::CSVIO::WriteCSV(path, vec);
    }

    return 0;
}
