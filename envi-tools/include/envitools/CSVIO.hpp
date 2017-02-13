#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

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
};
}
