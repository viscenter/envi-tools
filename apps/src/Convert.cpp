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

constexpr static float DEFAULT_GAMMA = 2.2f;
constexpr static uint8_t MAX_INTENSITY_8BPC =
    std::numeric_limits<uint8_t>::max();
constexpr static uint16_t MAX_INTENSITY_16BPC =
    std::numeric_limits<uint16_t>::max();

enum class BitDepth { Unsigned8 = 8, Unsigned16 = 16 };

int main(int argc, char* argv[])
{
    ///// Parse the cmd line /////
    fs::path imgPath, outputPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-path,i",po::value<std::string>()->required(),
            "Input image file")
        ("output-path,o", po::value<std::string>()->required(),
            "Output image file")
        ("bpc,b",po::value<int>()->default_value(16),
            "Output bit depth:\n"
            " 8  = 8bpc (unsigned)\n"
            " 16 = 16bpc (unsigned)")
        ("gamma", po::value<float>()->default_value(DEFAULT_GAMMA),
            "Gamma for floating point image tone mapping");
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

    // Parse the cmd line
    imgPath = parsedOptions["input-path"].as<std::string>();
    if (!fs::exists(imgPath)) {
        std::cerr << "ERROR: Input file does not exist" << std::endl;
        return EXIT_FAILURE;
    }

    outputPath = parsedOptions["output-path"].as<std::string>();
    auto depth = static_cast<BitDepth>(parsedOptions["bpc"].as<int>());

    // Open the image
    auto image = cv::imread(imgPath.string(), -1);

    // File checks
    if (!image.data) {
        std::cerr << "ERROR: Failed to open input image" << std::endl;
        return EXIT_FAILURE;
    }

    if (image.depth() != CV_32F) {
        std::cerr << "ERROR: Input image is not floating point" << std::endl;
        return EXIT_FAILURE;
    }

    // Tone map for contrast
    auto gamma = parsedOptions["gamma"].as<float>();
    image = et::ToneMap(image, gamma);

    // Scale to output bit depth
    switch (depth) {
        case BitDepth::Unsigned8:
            image *= MAX_INTENSITY_8BPC;
            image.convertTo(image, CV_8UC(image.channels()));
            break;
        case BitDepth::Unsigned16:
            image *= MAX_INTENSITY_16BPC;
            image.convertTo(image, CV_16UC(image.channels()));
            break;
    }

    // Save the output image
    cv::imwrite(outputPath.string(), image);
}
