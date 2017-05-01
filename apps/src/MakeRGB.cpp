#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ///// Parse the cmd line /////
    fs::path imgPathRed, imgPathGreen, imgPathBlue, outputPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("red,r",po::value<std::string>()->required(),
            "File path red channel image")
        ("green,g",po::value<std::string>()->required(),
            "File path green channel image")
        ("blue,b",po::value<std::string>()->required(),
            "File path blue channel image")
        ("output-file,o",po::value<std::string>()->required(),
            "Output file path. This program has no bit-depth scaling. Use an "
            "output format that supports your input bit-depth. e.g. Don't use "
            "JPG if your input images are 16bpc TIFFs.");
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
    imgPathRed = parsedOptions["red"].as<std::string>();
    imgPathGreen = parsedOptions["green"].as<std::string>();
    imgPathBlue = parsedOptions["blue"].as<std::string>();
    if ((!boost::filesystem::exists(imgPathRed)) ||
        (!boost::filesystem::exists(imgPathGreen)) ||
        (!boost::filesystem::exists(imgPathBlue))) {
        std::cerr << "ERROR: Input image file path(s) do not exist"
                  << std::endl;
        return EXIT_FAILURE;
    }

    // Get output file path
    outputPath = parsedOptions["output-file"].as<std::string>();

    // Load input images
    auto r = cv::imread(imgPathRed.string(), -1);
    auto g = cv::imread(imgPathGreen.string(), -1);
    auto b = cv::imread(imgPathBlue.string(), -1);

    // Verify format
    if (r.channels() != 1 || g.channels() != 1 || b.channels() != 1) {
        std::cerr << "ERROR: Input image not single channel" << std::endl;
        return EXIT_FAILURE;
    }

    if (r.depth() != g.depth() || r.depth() != b.depth()) {
        std::cerr << "ERROR: Input image depths do not match" << std::endl;
        return EXIT_FAILURE;
    }

    // Merge images
    cv::Mat outImage;
    cv::merge(std::vector<cv::Mat>{b, g, r}, outImage);

    // Replace extension if floating point
    if (outImage.depth() == CV_32F) {
        outputPath.replace_extension("hdr");
    }

    // Write output
    cv::imwrite(outputPath.string(), outImage);

    return EXIT_SUCCESS;
}
