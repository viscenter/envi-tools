//
// Created by Seth Parker on 11/7/17.
//
#include <iostream>

#include "envitools/ENVI.hpp"

namespace et = envitools;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: [input.hdr] \n";
        return EXIT_FAILURE;
    }

    std::cout << "Filename: " << argv[1] << std::endl;
    et::ENVI::Pointer envi;
    try {
        envi = et::ENVI::New(argv[1]);
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

    std::cout << "Band ID count: " << envi->getWavelengths().size()
              << std::endl;
    std::cout << "IDs:" << std::endl;
    int count = 0;
    for (const auto& w : envi->getWavelengths()) {
        std::cout << "    " << count++ << ": " << w << std::endl;
    }
}