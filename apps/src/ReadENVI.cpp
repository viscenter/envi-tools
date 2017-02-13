//
// Created by Seth Parker on 2/7/17.
//

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include "envitools/ENVI.hpp"

namespace et = envitools;
namespace fs = boost::filesystem;

int main(int /*argc*/, char* argv[])
{
    // Get args
    fs::path hdrPath = argv[1];
    int band = std::stoi(argv[2]);

    // Load ENVI file and print header info
    envitools::ENVI envi(hdrPath);
    envi.printHeader();

    // Get band from file
    auto m = envi.getBand(band);

    // Select file extension
    fs::path out = envi.getWavelength(band) + ".jpg";
    if (m.depth() == CV_32F) {
        out.replace_extension("hdr");
    } else if (m.depth() == CV_16U) {
        out.replace_extension("tif");
    }

    cv::imwrite(out.string(), m);
}
