#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "envitools/Box.hpp"
#include "envitools/CSVIO.hpp"
#include "envitools/ContrastMetrics.hpp"
#include "envitools/EnviUtils.hpp"

namespace fs = boost::filesystem;
namespace po = boost::program_options;
namespace et = envitools;
namespace cm = et::ContrastMetrics;

int main(int argc, char* argv[])
{
    ///// Parse the cmd line /////
    fs::path imgDir, csvPath, forePtsPath, backPtsPath, roiPath;

    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-dir,i",po::value<std::string>()->required(),
            "Input directory of wavelengths")
        ("foreground-pts,f",po::value<std::string>(),
            "Ink points for Michelson")
        ("background-pts,b",po::value<std::string>(),
            "Papyrus points for Michelson contrast")
        ("roi,r",po::value<std::string>(),
            "Region of interest for RMS contrast")
        ("output-file,o", po::value<std::string>()->required(),
            "Output file path");
    // clang-format on

    // parsedOptions will hold the values of all parsed options as a Map
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
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // Do we have the options to calculate contrast?
    auto doMichelson = parsedOptions.count("foreground-pts") > 0 &&
                       parsedOptions.count("background-pts") > 0;
    auto doRMS = parsedOptions.count("roi") > 0;

    // Stop if there's nothing to do
    if (!doMichelson && !doRMS) {
        std::cerr << "No arguments given for computing contrast" << std::endl;
        return EXIT_FAILURE;
    }

    // Get the output path
    csvPath = parsedOptions["output-file"].as<std::string>();

    ///// Collect the tif files /////
    imgDir = parsedOptions["input-dir"].as<std::string>();
    std::vector<fs::path> imgPaths = et::FindByExtension(imgDir, ".tif");
    if (imgPaths.empty()) {
        std::cerr << "Error opening/parsing image directory. No files found."
                  << std::endl;
        return EXIT_FAILURE;
    }

    ///// Setup Michelson Points and Regions ////
    std::vector<cv::Vec2i> forePts, backPts;
    if (doMichelson) {
        // Get the points file paths
        forePtsPath = parsedOptions["foreground-pts"].as<std::string>();
        backPtsPath = parsedOptions["background-pts"].as<std::string>();
        if (!boost::filesystem::exists(forePtsPath) ||
            !boost::filesystem::exists(backPtsPath)) {
            std::cerr << "Michelson points file does not exist" << std::endl;
            return EXIT_FAILURE;
        }
        // Load the pts
        forePts = et::CSVIO::ReadPointCSV(forePtsPath);
        backPts = et::CSVIO::ReadPointCSV(backPtsPath);
    }

    ///// Setup RMS Regions ////
    std::vector<et::Box> rmsBoxes;
    if (doRMS) {
        roiPath = parsedOptions["roi"].as<std::string>();
        if (!boost::filesystem::exists(roiPath)) {
            std::cerr << "Provided ROI file does not exist" << std::endl;
            return EXIT_FAILURE;
        }
        rmsBoxes = et::CSVIO::ReadROICSV(roiPath);
    }

    ///// Calculate contrast /////
    std::map<std::string, std::vector<double>> perWavelengthResults;
    std::vector<double> contrasts;
    cv::Mat image;
    std::cout << "Calculating contrast metrics..." << std::endl;
    for (auto path : imgPaths) {
        // Reset temp storage
        contrasts.clear();

        // Get wavelength for key
        auto id = et::ParseWavelength(path);
        std::cerr << "Wavelength: " << id << "\r";

        // Load the image
        image = cv::imread(path.string(), -1);
        if (image.empty()) {
            std::cout << "Could not open or find the image: ";
            std::cout << path.string() << std::endl;
            continue;
        }

        // Images are floating point. Tone map them to a reasonable scale
        image = et::ToneMap(image, float(2.2));

        // Michelson Contrast
        if (doMichelson) {
            contrasts.push_back(cm::MichelsonContrast(image, forePts, backPts));
        }

        // RMS Contrast
        if (doRMS) {
            for (auto box : rmsBoxes) {
                contrasts.push_back(cm::RMSContrast(image, box));
            }
        }

        // Add to the map
        perWavelengthResults[id] = contrasts;
    }
    std::cout << std::endl;

    ///// Make the CSV header /////
    std::vector<std::string> header;
    header.emplace_back("wavelength");
    if (doMichelson) {
        header.emplace_back("michel");
    }
    // Make one column for each RMS roi we loaded
    if (doRMS) {
        for (size_t i = 0; i < rmsBoxes.size(); i++) {
            header.emplace_back("rms" + std::to_string(i));
        }
    }

    ///// Write to a CSV /////
    std::cout << "Writing CSV..." << std::endl;
    et::CSVIO::WriteCSV(csvPath, perWavelengthResults, header);

    std::cout << "Done." << std::endl;
    return 0;
}
