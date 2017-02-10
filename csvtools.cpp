#include <iostream>
#include "csvtools.hpp"
#include <opencv2/core.hpp>
#include <fstream>

std::vector<cv::Vec2i> csv::read(string filename) {
	std::ifstream myfile;
	myfile.open(filename);

	while(myfile.good())
	{
		// save to std::vector
	}

	myfile.close();
	// return std::vector
}

void csv::write(std::vector<cv::Vec2i> points, string filename) {
	std::ofstream myfile;
	myfile.open(filename);

	for (auto i : points) {
		std::cout << points[0] << "," << points[1] << std::endl;
		myfile << points[0] << "," << points [1] << std::endl;
	}
	myfile.close();
}
