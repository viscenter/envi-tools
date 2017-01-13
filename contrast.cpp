#include <iostream>
#include <map>
#include <random>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

constexpr static uint16_t MAX_INTENSITY = std::numeric_limits<uint16_t>::max();

struct Box {
    Box() : xmin(0), xmax(0), ymin(0), ymax(0){};
    Box(int x_min, int y_min, int x_max, int y_max)
        : xmin(x_min), xmax(x_max), ymin(y_min), ymax(y_max){};
    int xmin, xmax, ymin, ymax;
};

// returns random int on interval [a,b]
int RandomInt(int a, int b)
{
    std::random_device genDevice;
    std::uniform_int_distribution<int> genDist(a, b);
    return genDist(genDevice);
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
    cv::Mat image, std::vector<cv::Vec2i> fg_pts, std::vector<cv::Vec2i> bg_pts)
{
    // Foreground average
    double fg_avg = 0.0;
    double s = 1.0 / fg_pts.size();
    for (auto i : fg_pts) {
        fg_avg += image.at<float>(i[1], i[0]) * s;
    }

    // Background average
    double bg_avg = 0.0;
    s = 1.0 / bg_pts.size();
    for (auto i : bg_pts) {
        bg_avg += image.at<float>(i[1], i[0]) * s;
    }

    // Contrast
    return std::abs((fg_avg - bg_avg) / (fg_avg + bg_avg));
}

// returns contrast of a subregion using root mean square metric
double RMSContrast(cv::Mat image, Box region)
{
    cv::Mat subImg = image(
        cv::Range(region.ymin, region.ymax),
        cv::Range(region.xmin, region.xmax));
    cv::Scalar m, s;
    cv::meanStdDev(subImg, m, s);

    return s[0];
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

cv::Mat ToneMap(cv::Mat m, float gamma = 1.0f)
{
    auto tmp = m.clone();
    cv::cvtColor(tmp, tmp, CV_GRAY2BGR);
    auto tonemap = cv::createTonemap(gamma);
    tonemap->process(tmp, tmp);
    cv::cvtColor(tmp, tmp, CV_BGR2GRAY);
    return tmp;
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
            << "rms0,"
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
    inkpts.push_back({847, 1300});
    inkpts.push_back({821, 1520});
    inkpts.push_back({777, 1530});
    inkpts.push_back({456, 1502});
    inkpts.push_back({355, 1041});
    inkpts.push_back({317, 896});
    inkpts.push_back({328, 494});
    inkpts.push_back({343, 406});
    inkpts.push_back({577, 1722});
    inkpts.push_back({627, 1712});
    inkpts.push_back({769, 1682});

    // Regions of papyrus with randomly generated points (for Michelson)
    std::vector<Box> PapyrusBoxes;
    PapyrusBoxes.push_back({464, 1512, 487, 1540});
    PapyrusBoxes.push_back({552, 1512, 576, 1569});
    PapyrusBoxes.push_back({694, 496, 715, 558});
    PapyrusBoxes.push_back({339, 539, 362, 574});
    PapyrusBoxes.push_back({852, 1285, 874, 1321});

    for (int i = 0; i < 500; i++) {
        auto box_id = RandomInt(0, static_cast<int>(PapyrusBoxes.size() - 1));
        auto box = PapyrusBoxes[box_id];

        auto x = RandomInt(box.xmin, box.xmax);
        auto y = RandomInt(box.ymin, box.ymax);
        papypts.push_back({x, y});
    }

    // Regions of ink and papyrus (for RMS)
    std::vector<Box> InkPapyrusBoxes;
    InkPapyrusBoxes.push_back({763, 1515, 828, 1549});
    InkPapyrusBoxes.push_back({670, 1483, 732, 1563});
    InkPapyrusBoxes.push_back({833, 1293, 852, 1328});
    InkPapyrusBoxes.push_back({872, 1284, 894, 1318});

    ///// Calculate contrast /////
    std::map<std::string, std::vector<double>> wavelength_results;
    std::vector<double> contrasts;
    cv::Mat image;
    std::cout << "Calculating contrast metrics..." << std::endl;
    for (auto path : imgpaths) {
        // Reset temp storage
        contrasts.clear();

        // Get wavelength for key
        auto id = ParseWavelength(path);
        std::cout << "Wavelength: " << id << "\r" << std::flush;

        // Load the image
        image = cv::imread(path.string(), -1);
        if (!image.data) {
            std::cout << "Could not open or find the image" << std::endl;
            std::cout << path.string() << std::endl;
            continue;
        }

        image = ToneMap(image, 2.2);

        // Michelson Contrast
        contrasts.push_back(MichelsonContrast(image, inkpts, papypts));

        // RMS Contrast
        for (auto box : InkPapyrusBoxes) {
            contrasts.push_back(RMSContrast(image, box));
        }

        // Add to the map
        wavelength_results[id] = contrasts;
    }
    std::cout << std::endl;

    ///// Write to a CSV /////
    std::cout << "Writing CSV..." << std::endl;
    WriteCSV(csv_path, wavelength_results);

    std::cout << "Done." << std::endl;
    return 0;
}
