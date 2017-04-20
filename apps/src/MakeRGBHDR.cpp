#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "envitools/EnviUtils.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace et = envitools;

int main(int argc, char* argv[])
{
    ///// Parse the cmd line /////
    fs::path imgPathRed, imgPathGreen, imgPathBlue, outputPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("img-path-red,r",po::value<std::string>()->required(),
            "File path of image for red channel")
        ("img-path-green,g",po::value<std::string>()->required(),
            "File path of image for green channel")
        ("img-path-blue,b",po::value<std::string>()->required(),
            "File path of image for blue channel")
        ("output-file,o",po::value<std::string>()->required(),
            "Output file path")
        ("gamma", po::value<float>()->default_value(2.2f),
            "Gamma for floating point image tone mapping");
    // clang-format on

    // parsedOptions will hold the values of all parsed options as a Map
    po::variables_map parsedOptions;
    po::store(
        po::command_line_parser(argc, argv).options(options).run(),
        parsedOptions);

    // show the help message
    if (parsedOptions.count("help") || argc < 5) {
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

    // Get input file paths
    imgPathRed = parsedOptions["img-path-red"].as<std::string>();
    imgPathGreen = parsedOptions["img-path-green"].as<std::string>();
    imgPathBlue = parsedOptions["img-path-blue"].as<std::string>();
    if ((!boost::filesystem::exists(imgPathRed)) ||
        (!boost::filesystem::exists(imgPathGreen)) ||
        (!boost::filesystem::exists(imgPathBlue))) {
        std::cerr << "ERROR: Input image file path(s) do not exist"
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Get output file path
    outputPath = parsedOptions["output-file"].as<std::string>();
    if (boost::filesystem::exists(outputPath)) {
        std::cerr << "ERROR: Output file path already exists. Cannot overwrite "
                     "output file."
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Load input images into respective cv::Mat
    cv::Mat r = cv::imread(imgPathRed.string(), -1);
    cv::Mat g = cv::imread(imgPathGreen.string(), -1);
    cv::Mat b = cv::imread(imgPathBlue.string(), -1);

    // Tone map for contrast
    auto gamma = parsedOptions["gamma"].as<float>();
    r = et::ToneMap(r, gamma);
    g = et::ToneMap(g, gamma);
    b = et::ToneMap(b, gamma);

    // Merge images
    std::vector<cv::Mat> imgArr;
    cv::Mat outImage;

    imgArr.push_back(b);
    imgArr.push_back(g);
    imgArr.push_back(r);

    cv::merge(imgArr, outImage);

    // Write new image to output path
    cv::imwrite(outputPath.string(), outImage);

    return 0;
}
