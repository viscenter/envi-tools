#pragma once

#include <array>
#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>

namespace envitools
{

/**
  @class ENVI
  @author Seth Parker
  @date 02/07/2017

  @brief ENVI file interface

  Reads ENVI files and streams band information from disk.

  More information at:
  <a href="https://www.harrisgeospatial.com/docs/ENVIHeaderFiles.html">ENVI
  Header Files</a>

  @ingroup envitools
*/
class ENVI
{
public:
    /** @brief Fundamental datatype options */
    enum class DataType {
        Unsigned8 = 1,
        Signed16,
        Signed32,
        Float32,
        Float64,
        Complex32,
        Complex64,
        Unsigned16 = 12,
        Unsigned32,
        Signed64,
        Unsigned64
    };

    /** @brief Endianness options */
    enum class Endianness { Little = 0, Big };

    /**
     * @brief Band interleave options
     *
     * Band interleave describes the ordering of pixel information inside of
     * the image data file. This option, along with the ENVI::DataType,
     * determines the exact byte position of any arbitrary pixel.
     *
     * More information at:
     * <a href="https://www.harrisgeospatial.com/docs/enviimagefiles.html">ENVI
     * Image Files</a>
     *
     * <b>Band Sequential</b>:
     * The most direct interleave option. Every band image is written
     * sequentially into the data file.
     *
     * <b>Band-interleaved-by-pixel</b>:
     * The first pixel of each band is written in order, followed by the
     * second pixel of each band, and so on.
     *
     * <b>Band-interleaved-by-line</b>:
     * The first line of each band is written in order, followed by the
     * second line of each band, and so on.
     *
     * @image html ENVI-BSQ.png height=500px
     * @image html ENVI-BIP.png height=500px
     * @image html ENVI-BIL.png height=500px
     * @image latex ENVI-BSQ.eps height=2in
     * @image latex ENVI-BIP.eps height=2in
     * @image latex ENVI-BIL.eps height=2in
     * */
    enum class Interleave { BandSequential, BandByPixel, BandByLine };

    /** @name Constructors */
    ///@{
    /** @brief Load from ENVI header file */
    ENVI(const boost::filesystem::path& header)
    {
        parse_header_(header);
        find_data_file_(header);
    }

    /** @brief Load from ENVI header and data files */
    ENVI(
        const boost::filesystem::path& header,
        const boost::filesystem::path& data)
        : dataPath_(data)
    {
        parse_header_(header);
    }
    ///@}

    /** @name Data Access */
    ///@{
    /** @brief Read specific band from ENVI file */
    cv::Mat getBand(int b);

    /** @brief Get wavelength of band as string */
    std::string getWavelength(int b) { return wavelengths_[b]; }

    /** @brief Get list of wavelengths */
    std::vector<std::string> getWavelengths() { return wavelengths_; }
    ///@}

    /** @name Metadata */
    ///@{
    /** @brief Get the ENVI::DataType of the file */
    DataType datatype() { return type_; }
    /** @brief Get the ENVI::Endianness of the file */
    Endianness endianness() { return endian_; }
    /** @brief Get the ENVI::Interleave of the file */
    Interleave interleave() { return interleave_; }
    /** @brief Get the width of the band images */
    int width() { return samples_; }
    /** @brief Get the height of the band images */
    int height() { return lines_; }
    /** @brief Get the number of band images */
    int bands() { return bands_; }
    ///@}

    /** @name Debug */
    ///@{
    /** @brief Print information parsed from header
     *
     * Prints to std::cerr. For debug purposes only.
     */
    void printHeader();
    ///@}

private:
    /** @brief Parse an ENVI header */
    void parse_header_(const boost::filesystem::path& header);

    /** @brief Attempt to find ENVI data file relative to header */
    void find_data_file_(const boost::filesystem::path& header);

    /**
     * @brief Return the byte position of pixel (x, y, band) in the ENVI data
     * file
     *
     * Position is dependent upon ENVI::Interleave and ENVI::DataType of the
     * ENVI file
     */
    uint64_t pos_of_elem_(uint64_t band, uint64_t y, uint64_t x, uint64_t size);

    /** ENVI file's fundamental datatype */
    DataType type_;
    /** ENVI file's endianess */
    Endianness endian_;
    /** ENVI file's band ordering */
    Interleave interleave_;
    /** Number of samples for each band (aka image width) */
    int samples_;
    /** Number of lines for each band (aka image height) */
    int lines_;
    /** Number of bands in ENVI file */
    int bands_;

    /** Path to ENVI data file */
    boost::filesystem::path dataPath_;

    /** List of parsed wavelengths */
    std::vector<std::string> wavelengths_;

    /**
     * @brief Read a specific band image from the ENVI data file
     *
     * Image is returned with its native bit depth.
     *
     * @tparam T Fundamental type of pixel data
     * @param b ID number of band to extract
     * @return Band image with pixel type T
     */
    template <typename T>
    cv::Mat get_band_(int b)
    {
        // Open the filestream
        std::ifstream ifs{dataPath_.string(), std::ios::binary};
        if (!ifs.is_open()) {
            throw std::runtime_error("Could not open ENVI data file");
        }

        // Setup output Mat
        cv::Mat_<T> output(lines_, samples_);

        // Size of each element
        auto length = sizeof(T);

        // Loop over every pixel in that band
        T value;
        std::ifstream::streampos pos;
        for (int y = 0; y < lines_; y++) {
            for (int x = 0; x < samples_; x++) {
                // Seek to the correct position in the data file
                pos = pos_of_elem_(
                    static_cast<uint64_t>(b), static_cast<uint64_t>(y),
                    static_cast<uint64_t>(x), length);
                ifs.seekg(pos);

                // Read the bytes
                ifs.read(reinterpret_cast<char*>(&value), length);
                if (ifs.fail()) {
                    auto msg = "Only read " + std::to_string(ifs.gcount()) +
                               " bytes. Expected: " + std::to_string(length);
                    throw std::runtime_error(msg);
                }

                // Assign to the mat
                output.template at<T>(y, x) = value;
            }
        }

        return output;
    }
};
}
