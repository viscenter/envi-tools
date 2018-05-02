// Convert an input tiff to the type that Skyscan likes to load
// Created by Seth Parker on 11/1/17.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "envitools/TIFFIO.hpp"

namespace et = envitools;

int main(int argc, char* argv[])
{
    if(argc != 3) {
        std::cerr << "Usage: [input.tif] [output.tif] \n";
        return EXIT_FAILURE;
    }

    auto input = cv::imread(argv[1], -1);
    et::TIFFIO::WriteTIFF(argv[2], input);
}