#pragma once

#include <opencv2/core.hpp>

namespace envitools
{
class ContrastMetrics
{
public:
    struct Box {
        Box() : xmin(0), xmax(0), ymin(0), ymax(0){};
        Box(int x_min, int y_min, int x_max, int y_max)
            : xmin(x_min), xmax(x_max), ymin(y_min), ymax(y_max){};
        int xmin, xmax, ymin, ymax;
    };

    static double MichelsonContrast(
        const cv::Mat& image,
        const std::vector<cv::Vec2i>& fg_pts,
        const std::vector<cv::Vec2i>& bg_pts);

    static double RMSContrast(const cv::Mat& image, const Box& region);
};
}
