#pragma once

#include <map>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

#include "envitools/Box.hpp"

namespace envitools
{
class CSVIO
{
public:
    // take filename, return vector of points
    static std::vector<cv::Vec2i> ReadPointCSV(boost::filesystem::path path);

    static std::vector<Box> ReadROICSV(boost::filesystem::path path);

    static void WriteCSV(
        boost::filesystem::path path, const std::vector<cv::Vec2i>& vec);

    static void WriteCSV(
        boost::filesystem::path path,
        const std::map<std::string, std::vector<double>>& res,
        const std::vector<std::string>& header = {});
};
}
