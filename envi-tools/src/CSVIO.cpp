#include "envitools/CSVIO.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

using namespace envitools;
namespace fs = boost::filesystem;

std::vector<cv::Vec2i> CSVIO::ReadPointCSV(fs::path path)
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

std::vector<Box> CSVIO::ReadROICSV(fs::path path)
{
    std::ifstream ifs(path.string());
    if (!ifs.good()) {
        throw std::runtime_error("File failed to open");
    }
    std::vector<Box> output;
    std::string line;
    std::vector<std::string> strs;
    while (std::getline(ifs, line)) {
        // Parse the line
        boost::trim(line);
        boost::split(strs, line, boost::is_any_of(","));
        std::for_each(std::begin(strs), std::end(strs), [](std::string& t) {
            boost::trim(t);
        });
        output.push_back({std::stoi(strs[0]), std::stoi(strs[1]),
                          std::stoi(strs[2]), std::stoi(strs[3])});
    }

    return output;
}

void CSVIO::WriteCSV(
    boost::filesystem::path path, const std::vector<cv::Vec2i>& vec)
{
    std::ofstream myfile;
    myfile.open(path.string());
    for (auto pt : vec) {
        myfile << pt[0] << "," << pt[1] << std::endl;
    }
    myfile.close();
}

void CSVIO::WriteCSV(
    fs::path path,
    const std::map<std::string, std::vector<double>>& res,
    const std::vector<std::string>& header)
{
    std::ofstream ofs(path.string());

    if (!ofs.good()) {
        throw std::runtime_error("File failed to open");
    }

    // Write the header
    for (auto h : header) {
        ofs << h << ",";
    }
    ofs << std::endl;

    // Write the data
    for (auto k : res) {
        // Wavelength
        ofs << k.first;

        // Contrast
        for (auto c : k.second) {
            ofs << "," << c;
        }
        ofs << std::endl;
    }

    // Close the file
    ofs.close();
}
