#include "envi-tools/CSVIO.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

static std::vector<cv::Vec2i> ReadCSV(boost::filesystem::path path)
{
    std::ifstream myfile;
    myfile.open(filename);

    while (myfile.good()) {
        // save to std::vector
    }

    myfile.close();
    // return std::vector
}

static void WriteCSV(
    const std::vector<cv::Vec2i>& points, boost::filesystem::path path)
{
    std::ofstream myfile;
    myfile.open(filename);

    for (auto i : points) {
        std::cout << points[0] << "," << points[1] << std::endl;
        myfile << points[0] << "," << points[1] << std::endl;
    }
    myfile.close();
}
