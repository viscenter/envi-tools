#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include "envitools/EnviUtils.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace et = envitools;

constexpr static uint16_t MAX_INTENSITY = std::numeric_limits<uint16_t>::max();

int main(int argc, char* argv[])
{

    ///// Parse the cmd line /////
    fs::path imgDir, outputPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-dir,i",po::value<std::string>()->required(),
            "Input directory of wavelengths")
        ("output-file,o", po::value<std::string>()->required(),
            "Output file path");
    // clang-format on

    // parsedOptions will hold the values of all parsed options as a Map
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
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output path
    outputPath = parsedOptions["output-file"].as<std::string>();

    ///// Collect the tif files /////
    imgDir = parsedOptions["input-dir"].as<std::string>();
    std::vector<fs::path> imgPaths = et::FindByExtension(imgDir, ".tif");
    if (imgPaths.empty()) {
        std::cerr << "Error opening/parsing image directory. No files found."
                  << std::endl;
        return EXIT_FAILURE;
    }

    ///// Convert
    cv::Mat image;
    std::cout << "Converting to 16bpc..." << std::endl;
    for (auto path : imgPaths) {
        // Get wavelength for key
        auto id = et::ParseWavelength(path);
        std::cout << "Wavelength: " << id << "\r" << std::flush;

        // Load the image
        image = cv::imread(path.string(), -1);
        if (!image.data) {
            std::cout << "Could not open or find the image" << std::endl;
            std::cout << path.string() << std::endl;
            continue;
        }

        image = et::ToneMap(image, 2.2);

        image *= MAX_INTENSITY;
        image.convertTo(image, CV_16UC1);
        fs::path outpath = outputPath / (id + ".tif");
        cv::imwrite(outpath.string(), image);
    }
    std::cout << std::endl;
}
