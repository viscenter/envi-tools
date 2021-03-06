//
// Created by Seth Parker on 2/7/17.
//

#include <numeric>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "envitools/ENVI.hpp"
#include "envitools/TIFFIO.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

std::vector<int> OptToBandList(const std::string& opt);

int main(int argc, char** argv)
{
    ///// Setup program options /////
    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-file,i", po::value<std::string>()->required(),
            "Path to the ENVI header file")
        ("bands,b", po::value<std::string>()->default_value("all"),
            "Comma-separated list of band index numbers to extract "
            "(e.g. \"0,56,27,133\"). If \"all\", program will extract all bands"
            " to the output directory.")
        ("output-dir,o", po::value<std::string>()->required(),
            "Output directory");
    // clang-format on

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

    ///// Get important paths /////
    fs::path enviPath, outputDir;
    enviPath = parsedOptions["input-file"].as<std::string>();
    if (!fs::exists(enviPath)) {
        std::cerr << "Error: File path does not exist." << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output path
    outputDir = parsedOptions["output-dir"].as<std::string>();
    if (!fs::is_directory(outputDir)) {
        std::cerr << "Error: Output path is not a directory." << std::endl;
        return EXIT_FAILURE;
    }

    ///// Load ENVI file /////
    envitools::ENVI envi(enviPath);
    
    ///// Build the list of bands we're going to extract /////
    auto bandsOpt = parsedOptions["bands"].as<std::string>();
    std::vector<int> bandsVec;
    // All bands
    if (bandsOpt == "all") {
        bandsVec.resize(static_cast<size_t>(envi.bands()));
        std::iota(bandsVec.begin(), bandsVec.end(), 0);
    }
    // List of bands
    else {
        bandsVec = OptToBandList(bandsOpt);
        std::sort(bandsVec.begin(), bandsVec.end());
    }

    ///// Do the processing /////
    // Prep the ENVI file for continuous access
    envi.setAccessMode(et::ENVI::AccessMode::KeepOpen);

    // Extract each band
    for (auto& band : bandsVec) {
        if (band < 0 || band >= envi.bands()) {
            std::cerr << "Error: Band (" << band << ") not in range. Skipping."
                      << std::endl;
            continue;
        }

        // Get band from file
        auto m = envi.getBand(band);

        // Select file extension
        fs::path out = outputDir / (envi.getWavelength(band) + ".png");

        if (m.depth() == CV_16U || m.depth() == CV_32F || m.depth() == CV_64F) {
            out.replace_extension("tif");
        }

        if (m.depth() == CV_32F || m.depth() == CV_64F) {
            et::TIFFIO::WriteTIFF(out, m);
        } else {
            cv::imwrite(out.string(), m);
        }
    }

    // Make sure the file gets closed
    envi.closeFile();
}

// Split a string (presumably comma separated) into a list of bands
std::vector<int> OptToBandList(const std::string& opt)
{
    // Setup output
    std::vector<int> output;

    // Split by Comma
    std::vector<std::string> splitBands;
    boost::split(splitBands, opt, boost::is_any_of(","));

    // Put in the output vector
    for (auto& b : splitBands) {
        try {
            output.emplace_back(std::stoi(b));
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return output;
}
