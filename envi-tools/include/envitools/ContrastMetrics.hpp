#pragma once

#include <opencv2/core.hpp>

#include "envitools/Box.hpp"

namespace envitools
{
namespace ContrastMetrics
{
    static double MichelsonContrast(
        const cv::Mat& image,
        const std::vector<cv::Vec2i>& fgPts,
        const std::vector<cv::Vec2i>& bgPts);

    static double RMSContrast(const cv::Mat& image, const Box& region);
    }
}
