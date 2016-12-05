#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <random>

#include <boost/filesystem.hpp>

using namespace cv;
using namespace std;

namespace fs = boost::filesystem;

// returns random int on interval [a,b]
int random(int a,int b) {
	srand(time(NULL));
	int c = b-a;
	int num = rand() % c + a;
	return num;
}

struct box {
	int xmin, xmax, ymin, ymax;	
};


// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
void get_all(const fs::path& root, const string& ext, vector<fs::path>& ret)
{
    if(!fs::exists(root) || !fs::is_directory(root)) return;

    fs::recursive_directory_iterator it(root);
    fs::recursive_directory_iterator endit;
    

    while(it != endit)
    {
        if(fs::is_regular_file(*it) && it->path().extension() == ext)
	{
	fs::path path = canonical(it->path());
	//ret.push_back(it->path().filename());
	ret.push_back(path);
	}
        ++it;

    }

}

// given path to image (wavelength).tif, return wavelength
string return_wavelength(fs::path imagePath) {
	auto name = imagePath.stem().string();

	int pos3 = name.find("-");
	if (pos3 !=string::npos)
	{
		name.replace(pos3,1,".");
	}
	
	return name;
}

// argv[1] == directory of wavelengths to be passed
int main(int argc, char** argv) {
	/*
	if (argc != 2)
	{
		cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}
	*/
	std::vector<fs::path> imgpaths;
	
	get_all(argv[1], ".tif", imgpaths);	

	std::vector<cv::Vec2i> inkpts, papypts;
        
	/* Random points of ink
	inkpts.push_back(cv::Vec2i(1703,658));
        inkpts.push_back(cv::Vec2i(1635,667));
        inkpts.push_back(cv::Vec2i(1706,584));
        inkpts.push_back(cv::Vec2i(1657,747));
        inkpts.push_back(cv::Vec2i(1545,539));

	inkpts.push_back(cv::Vec2i(1046,1177));
	inkpts.push_back(cv::Vec2i(1002,1183));
	inkpts.push_back(cv::Vec2i(1536,502));
	inkpts.push_back(cv::Vec2i(1498,497));
	inkpts.push_back(cv::Vec2i(1680,671));
	*/
	
	/* Regions of papyrus with randomly generated points	
	box papyrus_box, papyrus_box1, papyrus_box2, papyrus_box3;
	papyrus_box.xmin = 516;
	papyrus_box.xmax = 528;
	papyrus_box.ymax = 1561;
	papyrus_box.ymin = 1402;

	papyrus_box1.xmin = 723;
        papyrus_box1.xmax = 737;
        papyrus_box1.ymax = 1768;
        papyrus_box1.ymin = 1624;

	papyrus_box2.xmin = 559;
        papyrus_box2.xmax = 574;
        papyrus_box2.ymax = 1713;
        papyrus_box2.ymin = 1567;

	papyrus_box3.xmin = 773;
        papyrus_box3.xmax = 781;
        papyrus_box3.ymax = 1715;
        papyrus_box3.ymin = 1638;

	for (int i=0; i<500; i++)
	{
		papypts.push_back(cv::Vec2i(random(papyrus_box.ymin,papyrus_box.ymax),random(papyrus_box.xmin,papyrus_box.xmax)));
	}
	*/

	/* Random points of papyrus
        papypts.push_back(cv::Vec2i(1704,693));
        papypts.push_back(cv::Vec2i(1764,725));
        papypts.push_back(cv::Vec2i(1685,560));
        papypts.push_back(cv::Vec2i(1718,460));
        papypts.push_back(cv::Vec2i(1505,522));

        papypts.push_back(cv::Vec2i(1030,1059));
	papypts.push_back(cv::Vec2i(1187,990));
        papypts.push_back(cv::Vec2i(1528,459));
        papypts.push_back(cv::Vec2i(1044,1194));
        papypts.push_back(cv::Vec2i(1472,470));
	*/

	box inkPapy;
	inkPapy.xmin = 473;
	inkPapy.xmax = 532;
	inkPapy.ymin = 1429;
	inkPapy.ymax = 1559;

	cv::Mat image;
	
	for (auto paths : imgpaths)
	{
			
		image = imread(paths.string(),-1);	
		if (!image.data)
		{
			cout << "Could not open or find the image" << endl;
			cout << paths.string() << endl;
			return -1;
		}

		/*
		// Michelson Contrast Metric
		double ink_avg = 0.0;	

		for (auto i : inkpts)
		{
			ink_avg += image.at<float>(i[0], i[1]);
		}

		ink_avg /= inkpts.size();

		//cout << "Ink average: " << ink_avg << endl;
		
		double papy_avg = 0.0;

        	for (auto i : papypts)
        	{
        	        papy_avg += image.at<float>(i[0], i[1]);
        	}

        	papy_avg /= papypts.size();
		
		double contrast = abs((ink_avg - papy_avg)/(ink_avg + papy_avg));
		
		
		//cout << return_wavelength(paths) << "       " << contrast<< endl;
		//cout << return_wavelength(paths) << endl;
		//cout << contrast << endl;
		cout << ink_avg << endl;
		//cout << papy_avg << endl;
		*/
		
		// RMS Contrast
		
		// Normalize region for all intensities
		cv::Mat subImg = image(cv::Range(inkPapy.ymin,inkPapy.ymax), cv::Range(inkPapy.xmin,inkPapy.xmax)).clone();
		
		normalize(subImg,subImg);
		
		// compute average intensity
		double avgIntensity = 0.0;
		double imageSize = (inkPapy.ymax - inkPapy.ymin) * (inkPapy.xmax - inkPapy.xmin);
		
		cv::Scalar matMean = cv::mean(subImg);
		avgIntensity = matMean.val[0];
		
		// compute contrast
		double contrast = 0.0;
		for (int y = inkPapy.ymin; y < inkPapy.ymax; y++)
                {
                        for (int x = inkPapy.xmin; x < inkPapy.xmax; x++)
                        {
                        	contrast += pow((subImg.at<float>(y,x) - avgIntensity),2);
			}
                }

		contrast = sqrt((1/imageSize)*contrast);
		
		/* Check size of subImg	
		cv::Size s = subImg.size();
		int mat_size = s.height * s.width;

		cout << imageSize << "    " << mat_size << endl;
		*/
		
		//cout << return_wavelength(paths) << endl;
		cout << contrast << endl;
	
	}
	
	/*	
	namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display
	imshow("Display window", image); // Show image inside window
	
	waitKey(0);
	*/
	return 0;
}
