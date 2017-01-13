#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/photo.hpp>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

constexpr static uint16_t MAX_INTENSITY = std::numeric_limits<uint16_t>::max();

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

    while (it != endit) {
        if (fs::is_regular_file(*it) && it->path().extension() == ext) {
            fs::path path = canonical(it->path());
            ret.push_back(path);
        }
        ++it;
    }

    return ret;
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

// argv[1] == directory of wavelengths to be passed
int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << " Usage: " << argv[0];
        std::cout << " tif_directory/ output_dir/" << std::endl;
        return EXIT_FAILURE;
    }

    ///// Parse the cmd line /////
    fs::path img_dir = argv[1];
    fs::path out_dir = argv[2];

    ///// Collect the tif files /////
    std::vector<fs::path> imgpaths = FindByExtension(img_dir, ".tif");

    ///// Convert
    cv::Mat image;
    std::cout << "Converting to 16bpc..." << std::endl;
    for (auto path : imgpaths) {
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

        image *= MAX_INTENSITY;
        image.convertTo(image, CV_16UC1);
        fs::path outpath = out_dir / (id + ".tif");
        cv::imwrite(outpath.string(), image);
    }
    std::cout << std::endl;
}