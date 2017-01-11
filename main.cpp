#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <random>
#include <ctime>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

struct Box {
    int xmin, xmax, ymin, ymax;
};

// returns random int on interval [a,b]
int RandomInt(int a, int b)
{
    std::srand(std::time(0));
    int c = b - a;
    int num = rand() % c + a;
    return num;
}

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
std::vector<fs::path> FindByExtension(
    const fs::path root, const std::string ext)
{
    std::vector<fs::path> ret;
    if (!fs::exists(root) || !fs::is_directory(root)) {
        return ret;
    }

    fs::recursive_directory_iterator it(root);
    fs::recursive_directory_iterator endit;


    while(it != endit)
    {
        if (fs::is_regular_file(*it) && it->path().extension() == ext) {
            fs::path path = canonical(it->path());
            ret.push_back(path);
        }
        ++it;
    }

    return ret;
}

// returns contrast from set of points using michelson metric (Gene Ware)
double MichelsonContrast(
    cv::Mat image,
    std::vector<cv::Vec2i> inkpts,
    std::vector<cv::Vec2i> papypts)
{
    double ink_avg = 0.0;

    for (auto i : inkpts) {
        ink_avg += image.at<float>(i[0], i[1]);
    }

    ink_avg /= inkpts.size();

    double papy_avg = 0.0;

    for (auto i : papypts) {
        papy_avg += image.at<float>(i[0], i[1]);
    }

    papy_avg /= papypts.size();

    double contrast = std::abs((ink_avg - papy_avg) / (ink_avg + papy_avg));

    // cout << return_wavelength(paths) << "       " << contrast<< endl;
    // cout << return_wavelength(paths) << endl;
    // cout << contrast << endl;
    // cout << ink_avg << endl;
    // cout << papy_avg << endl;

    return contrast;
}

// returns contrast of a subregion using root mean square metric
double RMSContrast(cv::Mat image, Box region)
{
    cv::Mat subImg = image(
                         cv::Range(region.ymin, region.ymax),
                         cv::Range(region.xmin, region.xmax))
                         .clone();

    // normalize(subImg,subImg);
    double min, max;

    cv::minMaxLoc(subImg, &min, &max);

    subImg = (subImg - min) / (max - min);

    // compute average intensity
    double avgIntensity = 0.0;
    double imageSize =
        (region.ymax - region.ymin) * (region.xmax - region.xmin);

    cv::Scalar matMean = cv::mean(subImg);
    avgIntensity = matMean.val[0];

    // compute contrast
    double contrast = 0.0;
    for (int y = region.ymin; y < region.ymax; y++) {
        for (int x = region.xmin; x < region.xmax; x++) {
            contrast += std::pow((subImg.at<float>(y, x) - avgIntensity), 2);
        }
    }

    contrast = sqrt((1 / imageSize) * contrast);

    return contrast;
}

// given path to image (wavelength).tif, return wavelength
std::string ParseWavelength(fs::path imagePath)
{
    auto name = imagePath.stem().string();

    auto pos3 = name.find("-");
    if (pos3 != std::string::npos) {
        name.replace(pos3, 1, ".");
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
    std::vector<fs::path> imgpaths = FindByExtension(argv[1], ".tif");

    // std::vector<cv::Vec2i> inkpts, papypts;

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

    Box inkPapy0;
    inkPapy0.xmin = 473;
    inkPapy0.xmax = 532;
    inkPapy0.ymin = 1429;
    inkPapy0.ymax = 1559;

    Box inkPapy1;
    inkPapy1.xmin = 730;
    inkPapy1.xmax = 780;
    inkPapy1.ymin = 1640;
    inkPapy1.ymax = 1770;

    Box inkPapy2;
    inkPapy2.xmin = 516;
    inkPapy2.xmax = 605;
    inkPapy2.ymin = 1649;
    inkPapy2.ymax = 1699;

    Box inkPapy3;
    inkPapy3.xmin = 642;
    inkPapy3.xmax = 718;
    inkPapy3.ymin = 1672;
    inkPapy3.ymax = 1721;

    cv::Mat image;

    std::ofstream dataOutput;
    dataOutput.open("results.csv");

    dataOutput << "Wavelength,"
               << "Region0,"
               << "Region1,"
               << "Region2,"
               << "Region3" << std::endl
               << std::endl;
    std::cout << "Wavelength,"
              << "Region0,"
              << "Region1,"
              << "Region2,"
              << "Region3" << std::endl
              << std::endl;

    for (auto paths : imgpaths) {

        image = cv::imread(paths.string(), -1);
        if (!image.data) {
            std::cout << "Could not open or find the image" << std::endl;
            std::cout << paths.string() << std::endl;
            return -1;
        }

        double contrast0 = RMSContrast(image, inkPapy0);
        double contrast1 = RMSContrast(image, inkPapy1);
        double contrast2 = RMSContrast(image, inkPapy2);
        double contrast3 = RMSContrast(image, inkPapy3);

        // cout << return_wavelength(paths) << endl;
        dataOutput << ParseWavelength(paths) << "," << contrast0 << ","
                   << contrast1 << "," << contrast2 << "," << contrast3
                   << std::endl;

        std::cout << ParseWavelength(paths) << "," << contrast0 << ","
                  << contrast1 << "," << contrast2 << "," << contrast3
                  << std::endl;
    }

    dataOutput.close();

    /*
    namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for
    display
    imshow("Display window", image); // Show image inside window

    waitKey(0);
    */

    return 0;
}
