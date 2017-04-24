/**
 * @file TIFFIO.hpp
 * @brief IO Utilities for TIFF files
 * @author Seth Parker
 * @date 4/14/17
 *
 * @ingroup io
 */

#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace envitools
{
namespace TIFFIO
{
/**
 * @brief Write a TIFF image to file
 *
 * Supports writing floating point TIFFs, in addition to 8 & 16 bit integral
 * types. Currently only supports single-channel images. Unless you need to
 * write a floating point image, using cv::imwrite() is a better option.
 */
void WriteTIFF(const boost::filesystem::path& path, const cv::Mat& img);
}
}