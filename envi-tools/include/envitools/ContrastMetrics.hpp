#pragma once

#include <opencv2/core.hpp>
#include "envitools/Box.hpp"

namespace envitools
{
class ContrastMetrics
{
public:
    static double MichelsonContrast(
        const cv::Mat& image,
        const std::vector<cv::Vec2i>& fg_pts,
        const std::vector<cv::Vec2i>& bg_pts);

    static double RMSContrast(const cv::Mat& image, const Box& region);
};
}
