#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace envitools
{
static cv::Mat FlatfieldCorrection(
    const cv::Mat& input, const cv::Mat& dark, const cv::Mat& white)
{
    // Clone input
    auto output = input.clone();
    auto darkScaled = dark.clone();
    auto whiteScaled = white.clone();

    // Scale the flatfields
    if (dark.size != output.size) {
        cv::resize(darkScaled, darkScaled, {output.cols, output.rows});
    }
    if (white.size != output.size) {
        cv::resize(whiteScaled, whiteScaled, {output.cols, output.rows});
    }

    // Apply correction
    return (output - darkScaled) / (whiteScaled - darkScaled);
}
}  // namespace envitools
