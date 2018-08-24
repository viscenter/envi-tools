//
// Created by Seth Parker on 11/7/17.
//
#include <iostream>

#include <boost/program_options.hpp>

#include "envitools/ENVI.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ///// Setup program options /////
    // clang-format off
    po::options_description options("Options");
    options.add_options()
        ("help,h","Show this message")
        ("input-file,i", po::value<std::string>()->required(),
            "Path to the ENVI header file")
        ("print-band-ids", "If enabled, print the band IDs");
    // clang-format on

    po::variables_map parsed;
    po::store(
        po::command_line_parser(argc, argv).options(options).run(), parsed);

    // show the help message
    if (parsed.count("help") || argc < 2) {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    // warn of missing options
    try {
        po::notify(parsed);
    } catch (po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    ///// Get important paths /////
    fs::path enviPath;
    enviPath = parsed["input-file"].as<std::string>();
    if (!fs::exists(enviPath)) {
        std::cerr << "Error: File path does not exist." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Filename: " << enviPath << std::endl;
    et::ENVI::Pointer envi;
    try {
        envi = et::ENVI::New(enviPath);
    } catch (const std::exception& e) {
        std::cerr << "Cannot open file as ENVI: ";
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "DataType: " << static_cast<int>(envi->datatype())
              << std::endl;

    std::cout << "Endianness: " << static_cast<int>(envi->endianness())
              << std::endl;

    std::cout << "Interleave: ";
    switch (envi->interleave()) {
        case et::ENVI::Interleave::BandSequential:
            std::cout << "BSQ";
            break;
        case et::ENVI::Interleave::BandByPixel:
            std::cout << "BIP";
            break;
        case et::ENVI::Interleave::BandByLine:
            std::cout << "BIL";
            break;
    }
    std::cout << std::endl;

    std::cout << "Samples (Width): " << envi->width() << std::endl;
    std::cout << "Lines (Height): " << envi->height() << std::endl;
    std::cout << "Bands (Depth): " << envi->bands() << std::endl;

    // Print Band IDs
    std::cout << "Band ID count: " << envi->getWavelengths().size()
              << std::endl;
    if (parsed.count("print-band-ids") > 0) {
        std::cout << "IDs:" << std::endl;
        int count = 0;
        for (const auto& w : envi->getWavelengths()) {
            std::cout << "  " << count++ << ": " << w << std::endl;
        }
    }
}
