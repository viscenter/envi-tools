#include <fstream>
#include <iostream>
#include <random>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "envitools/CSVIO.hpp"
#include "envitools/EnviUtils.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace et = envitools;

int main(int argc, char** argv) {
    fs::path inputPath, csvPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h", "Show this message")
        ("input-file,i", po::value<std::string>()->required(),
            "Input file path of ROIs")
        ("output-file,o", po::value<std::string>()->required(),
            "Output file path")
        ("point-count,p", po::value<int>()->required(),
            "total number of points to generate");
    // clang-format on

    // parsedOptions will hold the value of all parsed options as a Map
    po::variables_map parsedOptions;
    po::store(
        po::command_line_parser(argc, argv).options(options).run(),
        parsedOptions);

    // show the help message
    if (parsedOptions.count("help") || argc < 4) {
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

    // Get the ROI CSV file path
    inputPath = parsedOptions["input-file"].as<std::string>();

    if (!(boost::filesystem::exists(inputPath))) {
        std::cerr << "Error: File path does not exist." << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output path
    csvPath = parsedOptions["output-file"].as<std::string>();

    if (boost::filesystem::exists(csvPath)) {
        std::cerr
            << "Error: File path already exists. Cannot overwrite output file."
            << std::endl;
        return EXIT_FAILURE;
    }

    // Read ROIs into vector
    auto vecROIs = et::CSVIO::ReadROICSV(inputPath);

    // Get number of points to generate
    auto numPoints = parsedOptions["point-count"].as<int>();

    std::cout << "X,Y" << std::endl;
    std::vector<cv::Vec2i> vec;
    for (int i = 0; i < numPoints; i++) {
        auto roi = vecROIs[envitools::RandomInt(0, vecROIs.size() - 1)];
        auto x = envitools::RandomInt(roi.xmin, roi.xmax);
        auto y = envitools::RandomInt(roi.ymin, roi.ymax);
        vec.emplace_back(x, y);
        std::cout << x << "," << y << std::endl;
    }

    envitools::CSVIO::WriteCSV(csvPath, vec);

    return 0;
}
