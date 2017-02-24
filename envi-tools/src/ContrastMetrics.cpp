#include "envitools/ContrastMetrics.hpp"

using namespace envitools;

double ContrastMetrics::MichelsonContrast(
    const cv::Mat& image,
    const std::vector<cv::Vec2i>& fgPts,
    const std::vector<cv::Vec2i>& bgPts)
{
    // Foreground average
    double fgAvg = 0.0;
    double sizeReciprocal = 1.0 / fgPts.size();
    for (auto pt : fgPts) {
        fgAvg += image.at<float>(pt[1], pt[0]) * sizeReciprocal;
    }

    // Background average
    double bgAvg = 0.0;
    sizeReciprocal = 1.0 / bgPts.size();
    for (auto pt : bgPts) {
        bgAvg += image.at<float>(pt[1], pt[0]) * sizeReciprocal;
    }

    // Contrast
    return std::abs((fgAvg - bgAvg) / (fgAvg + bgAvg));
}

double ContrastMetrics::RMSContrast(const cv::Mat& image, const Box& region)
{
    cv::Mat subImg = image(
        cv::Range(region.ymin, region.ymax),
        cv::Range(region.xmin, region.xmax));
    cv::Scalar mean, stdDev;
    cv::meanStdDev(subImg, mean, stdDev);

    return stdDev[0];
}
