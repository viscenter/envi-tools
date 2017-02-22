#pragma once

#include <map>
#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace envitools
{
class CSVIO
{
public:
    // take filename, return vector of points
    static std::vector<cv::Vec2i> ReadCSV(boost::filesystem::path path);

    // take filename and vector, create csv file
    static void WriteCSV(
        boost::filesystem::path path, const std::vector<cv::Vec2i>& points);

    static void WriteCSV(
        boost::filesystem::path path,
        std::map<std::string, std::vector<double>>& res);
};
}
