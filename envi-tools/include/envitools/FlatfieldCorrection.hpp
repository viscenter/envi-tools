#pragma once

#include <memory>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

namespace envitools
{
static cv::Mat FlatfieldCorrection(
    const cv::Mat& input, const cv::Mat& dark, const cv::Mat& flat)
{
    if (input.depth() != dark.depth() || input.depth() != flat.depth()) {
        throw std::runtime_error(
            "Input image does not have same bit depth as dark/flat-field "
            "image.");
    }

    // Do floating point math
    cv::Mat output, darkScaled, flatScaled;
    input.convertTo(output, CV_32F);
    dark.convertTo(darkScaled, CV_32F);
    flat.convertTo(flatScaled, CV_32F);

    // Scale the flatfields
    if (dark.size != output.size) {
        cv::resize(darkScaled, darkScaled, {output.cols, output.rows});
    }
    if (flat.size != output.size) {
        cv::resize(flatScaled, flatScaled, {output.cols, output.rows});
    }

    // Apply correction
    output -= darkScaled;
    output /= (flatScaled - darkScaled);

    return output;
}
}  // namespace envitools
