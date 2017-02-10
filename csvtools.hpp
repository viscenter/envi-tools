#ifndef CSVTOOLS_H
#define CSVTOOLS_H

class csv {

public:
	csv();

	// take filename, return vector of points
	std::vector<cv::Vec2i> csv::read(string filename) ;

	// take vector, filename, create csv file
	void csv::write(std::vector<cv::Vec2i> points, string filename);
	
#endif
