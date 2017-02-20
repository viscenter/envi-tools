#pragma once

#include <map>
#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace fs = boost::filesystem;

namespace envitools
{
class CSVIO
{
public:
    // take filename, return vector of points
    static std::vector<cv::Vec2i> ReadCSV(boost::filesystem::path path);

    // take vector, filename, create csv file
    static void WriteCSV(
        const std::vector<cv::Vec2i>& points, boost::filesystem::path path);

    static void WriteCSV(
        fs::path path, std::map<std::string, std::vector<double>> res);
};
}
