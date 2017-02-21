#pragma once

#include <random>
#include <boost/filesystem.hpp>
#include <opencv2/photo.hpp>

namespace fs = boost::filesystem;

namespace envitools
{

// given path to image (wavelength).tif, return wavelength
static std::string ParseWavelength(fs::path imagePath)
{
    auto name = imagePath.stem().string();

    auto pos3 = name.find("-");
    if (pos3 != std::string::npos) {
        name.replace(pos3, 1, ".");
    }

    return name;
}

static cv::Mat ToneMap(cv::Mat m, float gamma = 1.0f)
{
    auto tmp = m.clone();
    cv::cvtColor(tmp, tmp, CV_GRAY2BGR);
    auto tonemap = cv::createTonemap(gamma);
    tonemap->process(tmp, tmp);
    cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
    return tmp;
}

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
static std::vector<fs::path> FindByExtension(
    const fs::path root, const std::string ext)
{
    std::vector<fs::path> ret;
    if (!fs::exists(root) || !fs::is_directory(root)) {
        return ret;
    }

    fs::recursive_directory_iterator it(root);
    fs::recursive_directory_iterator endit;

    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().extension() == ext) {
            fs::path path = canonical(it->path());
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
};
}
