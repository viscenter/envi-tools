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

    static void WriteCSV(
        boost::filesystem::path path,
        const std::map<std::string, std::vector<double>>& res,
        const std::vector<std::string>& header = {});
};
}
