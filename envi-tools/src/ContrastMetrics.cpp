#include "envitools/ContrastMetrics.hpp"

using namespace envitools;

double ContrastMetrics::MichelsonContrast(
    const cv::Mat& image,
    const std::vector<cv::Vec2i>& fg_pts,
    const std::vector<cv::Vec2i>& bg_pts)
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

double ContrastMetrics::RMSContrast(
    const cv::Mat& image, const ContrastMetrics::Box& region)
{
    cv::Mat subImg = image(
        cv::Range(region.ymin, region.ymax),
        cv::Range(region.xmin, region.xmax));
    cv::Scalar m, s;
    cv::meanStdDev(subImg, m, s);

    return s[0];
}
