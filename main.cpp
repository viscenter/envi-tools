#include <iostream>
#include <map>
#include <random>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

struct Box {
    Box() : xmin(0), xmax(0), ymin(0), ymax(0){};
    Box(int x_min, int x_max, int y_min, int y_max)
        : xmin(x_min), xmax(x_max), ymin(y_min), ymax(y_max){};
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
        ink_avg += image.at<float>(i[1], i[0]);
    }

    ink_avg /= inkpts.size();

    double papy_avg = 0.0;

    for (auto i : papypts) {
        papy_avg += image.at<float>(i[1], i[0]);
    }

    papy_avg /= papypts.size();

    double contrast = std::abs((ink_avg - papy_avg) / (ink_avg + papy_avg));

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

void WriteCSV(fs::path path, std::map<std::string, std::vector<double>> res)
{
    std::ofstream ofs;
    ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        ofs.open(path.string());

        // Write the header
        ofs << "wavelength,"
            << "michel,"
            << "rms1,"
            << "rms2,"
            << "rms3" << std::endl;

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
    } catch (std::ifstream::failure e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}

// argv[1] == directory of wavelengths to be passed
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << " Usage: " << argv[0];
        std::cout << " tif_directory/ output.csv" << std::endl;
        return EXIT_FAILURE;
    }

    ///// Parse the cmd line /////
    fs::path img_dir = argv[1];
    fs::path csv_path = argv[2];

    ///// Collect the tif files /////
    std::vector<fs::path> imgpaths = FindByExtension(img_dir, ".tif");

    ///// Setup Sample Points and Regions ////
    std::vector<cv::Vec2i> inkpts, papypts;

    // Ink points (for Michelson)
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

    // Regions of papyrus with randomly generated points (for Michelson)
    std::vector<Box> PapyrusBoxes;
    PapyrusBoxes.push_back({516, 528, 1561, 1402});
    PapyrusBoxes.push_back({723, 737, 1768, 1624});
    PapyrusBoxes.push_back({559, 574, 1713, 1567});
    PapyrusBoxes.push_back({773, 781, 1715, 1638});

    for (int i = 0; i < 500; i++) {
        auto box_id = RandomInt(0, static_cast<int>(PapyrusBoxes.size() - 1));
        auto box = PapyrusBoxes[box_id];

        auto x = RandomInt(box.xmin, box.xmax);
        auto y = RandomInt(box.ymin, box.ymax);
        papypts.push_back({x, y});
    }

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

    // Regions of ink and papyrus (for RMS)
    std::vector<Box> InkPapyrusBoxes;
    InkPapyrusBoxes.push_back({473, 532, 1429, 1559});
    InkPapyrusBoxes.push_back({730, 780, 1640, 1770});
    InkPapyrusBoxes.push_back({516, 605, 1649, 1699});
    InkPapyrusBoxes.push_back({642, 718, 1672, 1721});

    ///// Calculate contrast /////
    std::map<std::string, std::vector<double>> wavelength_results;
    std::vector<double> contrasts;
    cv::Mat image;
    for (auto path : imgpaths) {
        // Reset temp storage
        contrasts.clear();

        // Get wavelength for key
        auto id = ParseWavelength(path);

        // Load the image
        image = cv::imread(path.string(), -1);
        if (!image.data) {
            std::cout << "Could not open or find the image" << std::endl;
            std::cout << path.string() << std::endl;
            continue;
        }

        // Normalize the image
        // To-Do

        // Michelson Contrast
        contrasts.push_back(MichelsonContrast(image, inkpts, papypts));

        // RMS Contrast
        for (auto box : InkPapyrusBoxes) {
            contrasts.push_back(RMSContrast(image, box));
        }

        // Add to the map
        wavelength_results[id] = contrasts;
    }

    ///// Write to a CSV /////
    WriteCSV(csv_path, wavelength_results);

    return 0;
}
