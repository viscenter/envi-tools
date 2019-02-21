#pragma once

#include <random>

#include <boost/filesystem.hpp>
#include <opencv2/photo.hpp>

namespace envitools
{

// given path to image (wavelength).tif, return wavelength
static std::string ParseWavelength(boost::filesystem::path imagePath)
{
    auto name = imagePath.stem().string();

    auto pos3 = name.find("-");
    if (pos3 != std::string::npos) {
        name.replace(pos3, 1, ".");
    }

    return name;
}

static cv::Mat ToneMap(const cv::Mat& m, float gamma = 1.0f)
{
    // Make a working copy
    auto tmp = m.clone();

    // Tone map requires 3-channel images
    if (m.channels() == 1) {
        cv::cvtColor(tmp, tmp, cv::COLOR_GRAY2BGR);
    }

    // Tonemap
    auto tonemap = cv::createTonemap(gamma);
    tonemap->process(tmp, tmp);

    // Convert back to gray if it was gray
    if (m.channels() == 1) {
        cv::cvtColor(tmp, tmp, cv::COLOR_BGR2GRAY);
    }

    return tmp;
}

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
static std::vector<boost::filesystem::path> FindByExtension(
    const boost::filesystem::path root, const std::string ext)
{
    std::vector<boost::filesystem::path> ret;
    if (!boost::filesystem::exists(root) ||
        !boost::filesystem::is_directory(root)) {
        return ret;
    }

    boost::filesystem::recursive_directory_iterator it(root);
    boost::filesystem::recursive_directory_iterator endit;

    while (it != endit) {
        if (boost::filesystem::is_regular_file(*it) &&
            it->path().extension() == ext) {
            boost::filesystem::path path = canonical(it->path());
            ret.push_back(path);
        }
        ++it;
    }

    return ret;
}

static int RandomInt(int a, int b)
{
    std::random_device genDevice;
    std::uniform_int_distribution<int> genDist(a, b);
    return genDist(genDevice);
}
}
