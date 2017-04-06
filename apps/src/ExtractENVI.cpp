//
// Created by Seth Parker on 2/7/17.
//

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "envitools/ENVI.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    fs::path hdrPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-file,i", po::value<std::string>()->required(),
            "Path to the ENVI header file")
        ("band,b", po::value<int>()->required(),
            "Band to extract")
        ("output-dir,o", po::value<std::string>()->required(),
            "Output directory");
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

    // Get the hdr path
    hdrPath = parsedOptions["input-file"].as<std::string>();

    if (!(boost::filesystem::exists(hdrPath))) {
        std::cerr << "Error: File path does not exist." << std::endl;
        return EXIT_FAILURE;
    }

    // Get band info
    auto band = parsedOptions["band"].as<int>();

    // Get args
    // fs::path hdrPath = argv[1];
    // int band = std::stoi(argv[2]);

    // Load ENVI file and print header info
    envitools::ENVI envi(hdrPath);
    envi.printHeader();

    // Get band from file
    auto m = envi.getBand(band);

    // Select file extension
    fs::path out = envi.getWavelength(band) + ".png";
    if (m.depth() == CV_32F) {
        out.replace_extension("hdr");
    } else if (m.depth() == CV_16U) {
        out.replace_extension("tif");
    }

    cv::imwrite(out.string(), m);
}
