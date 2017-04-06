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

constexpr static uint8_t MAX_INTENSITY_8BPC = std::numeric_limits<uint8_t>::max();
constexpr static uint16_t MAX_INTENSITY_16BPC =
    std::numeric_limits<uint16_t>::max();

enum class BitDepth { Unsigned8 = 8, Unsigned16 = 16 };

int main(int argc, char* argv[])
{
    ///// Parse the cmd line /////
    fs::path imgDir, outputPath;
    std::cout.width(10);

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-dir,i",po::value<std::string>()->required(),
            "Input directory")
        ("output-dir,o", po::value<std::string>()->required(),
            "Output directory")
        ("bpc,b",po::value<int>()->default_value(16),
            "Output bit depth:\n"
            " 8  = 8bpc (unsigned)\n"
            " 16 = 16bpc (unsigned)")
        ("gamma", po::value<float>()->default_value(2.2f),
            "Gamma for floating point image tone mapping");
    // clang-format on

    // parsedOptions will hold the values of all parsed options as a Map
    po::variables_map parsedOptions;
    po::store(
        po::command_line_parser(argc, argv).options(options).run(),
        parsedOptions);

    // show the help message
    if (parsedOptions.count("help") || argc < 3) {
        std::cout << argv[0] << std::endl;
        std::cout.fill('-');
        std::cout.width(20);
        std::cout << "\n";
        std::cout << "A utility for converting a directory of 32bpc, "
                     "floating-point TIFFs into" << std::endl;
        std::cout << "8 or 16bpc TIFFs." << std::endl;
        std::cout << "\n";
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

    // Get the output options
    outputPath = parsedOptions["output-file"].as<std::string>();
    auto depth = static_cast<BitDepth>(parsedOptions["bit-depth"].as<int>());

    ///// Collect the tif files /////
    imgDir = parsedOptions["input-dir"].as<std::string>();
    auto imgPaths = et::FindByExtension(imgDir, ".tif");
    if (imgPaths.empty()) {
        std::cerr << "Error opening/parsing image directory. No files found."
                  << std::endl;
        return EXIT_FAILURE;
    }

    ///// Convert /////
    cv::Mat image;
    std::cout << "Converting to " << static_cast<int>(depth) << " bpc..."
              << std::endl;
    for (auto path : imgPaths) {
        // Get wavelength for key
        std::string id = path.stem().string();
        std::cout << "Image: " << id << "\r" << std::flush;

        // Load the image
        image = cv::imread(path.string(), -1);
        if (!image.data) {
            std::cout << "Could not open or find the image" << std::endl;
            std::cout << path.string() << std::endl;
            continue;
        }

        // Tone map for contrast
        auto gamma = parsedOptions["gamma"].as<float>();
        image = et::ToneMap(image, gamma);

        // Scale to output bit depth
        switch (depth) {
            case BitDepth::Unsigned8:
                image *= MAX_INTENSITY_8BPC;
                image.convertTo(image, CV_8UC1);
                break;
            case BitDepth::Unsigned16:
                image *= MAX_INTENSITY_16BPC;
                image.convertTo(image, CV_16UC1);
                break;
        }

        // Save the output image
        fs::path outpath = outputPath / (id + ".tif");
        cv::imwrite(outpath.string(), image);
    }
    std::cout << std::endl;
}
