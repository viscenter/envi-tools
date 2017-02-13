#include "envitools/CSVIO.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace envitools;
namespace fs = boost::filesystem;

std::vector<cv::Vec2i> CSVIO::ReadCSV(fs::path path)
{
    std::ifstream ifs(path.string());
    if (!ifs.good()) {
        throw std::runtime_error("File failed to open");
    }
    std::vector<cv::Vec2i> output;
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(ifs, line)) {
        // Parse the line
        boost::trim(line);
        boost::split(strs, line, boost::is_any_of(","));
        std::for_each(std::begin(strs), std::end(strs), [](std::string& t) {
            boost::trim(t);
        });
        output.emplace_back(std::stoi(strs[0]), std::stoi(strs[1]));
    }

    return output;
}

void CSVIO::WriteCSV(const std::vector<cv::Vec2i>& points, fs::path path)
{
    std::ofstream myfile;
    myfile.open(path.string());

    for (auto i : points) {
        std::cout << points[0] << "," << points[1] << std::endl;
        myfile << points[0] << "," << points[1] << std::endl;
    }
    myfile.close();
}
