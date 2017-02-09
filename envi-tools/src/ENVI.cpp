#include "envi-tools/ENVI.hpp"

#include <regex>

#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;
using namespace envitools;

// Read an ENVI header file
void ENVI::parse_header_(const fs::path& header)
{
    // Open file
    std::ifstream ifs(header.string());
    if (!ifs.good()) {
        throw std::runtime_error("Cannot open header");
    }

    // Lines we know how to handle
    std::regex envi{"^ENVI"};
    std::regex type{"^data type"};
    std::regex endian{"^byte order"};
    std::regex interleave("^interleave");
    std::regex samples("^samples");
    std::regex lines("^lines");
    std::regex bands("^bands");
    std::regex bandIDs("^wavelength");

    // Start parsing
    std::string line;

    // Check first line says ENVI
    std::getline(ifs, line);
    boost::trim(line);
    if (!std::regex_match(line, envi)) {
        throw std::runtime_error("File is not an ENVI header file");
    }

    // Begin parsing loop
    std::vector<std::string> strs;
    while (std::getline(ifs, line)) {
        // Split the line by =
        boost::trim(line);
        boost::split(strs, line, boost::is_any_of("="));
        std::for_each(std::begin(strs), std::end(strs), [](std::string& t) {
            boost::trim(t);
        });

        // Handle data type
        if (std::regex_match(strs[0], type)) {
            type_ = static_cast<DataType>(std::stoi(strs[1]));
        }

        // Handle byte order
        else if (std::regex_match(strs[0], endian)) {
            endian_ = static_cast<Endianness>(std::stoi(strs[1]));
        }

        // Handle interleave
        else if (std::regex_match(strs[0], interleave)) {
            if (strs[1] == "bsq") {
                interleave_ = Interleave::BandSequential;
            } else if (strs[1] == "bip") {
                interleave_ = Interleave::BandByPixel;
            } else if (strs[1] == "bil") {
                interleave_ = Interleave::BandByLine;
            } else {
                throw std::runtime_error("Unrecognized interleave type");
            }
        }

        // Handle dimensions
        else if (std::regex_match(strs[0], samples)) {
            samples_ = std::stoi(strs[1]);
        } else if (std::regex_match(strs[0], lines)) {
            lines_ = std::stoi(strs[1]);
        } else if (std::regex_match(strs[0], bands)) {
            bands_ = std::stoi(strs[1]);
        }

        // Handle band IDs
        else if (std::regex_match(strs[0], bandIDs)) {
            std::vector<std::string> substr;

            std::getline(ifs, line);
            boost::trim(line);
            while (line != "}") {
                boost::trim_if(line, boost::is_any_of(","));
                wavelengths_.push_back(line);

                std::getline(ifs, line);
                boost::trim(line);
            }
        }

        // Clear and continue
        strs.clear();
    }

    ifs.close();
}

// Find the image data file relative to the header location
void ENVI::find_data_file_(const boost::filesystem::path& header)
{
    // File should have same name and no extension
    fs::path tmp = header;
    tmp.replace_extension("");
    if (fs::exists(tmp)) {
        dataPath_ = tmp;
        return;
    }

    throw std::runtime_error("Data file not found. Please specify manually");
}

// Get a specific band from the ENVI file
cv::Mat ENVI::getBand(int b)
{
    switch (type_) {
        case DataType::Unsigned8:
            return get_band_<uint8_t>(b);
        case DataType::Signed16:
            return get_band_<int16_t>(b);
        case DataType::Signed32:
            return get_band_<int32_t>(b);
        case DataType::Float32:
            return get_band_<float>(b);
        case DataType::Float64:
            return get_band_<double>(b);
        case DataType::Complex32:
            throw std::runtime_error("Complex ENVI files are unsupported");
        case DataType::Complex64:
            throw std::runtime_error("Complex ENVI files are unsupported");
        case DataType::Unsigned16:
            return get_band_<uint16_t>(b);
        case DataType::Unsigned32:
            return get_band_<uint32_t>(b);
        case DataType::Signed64:
            return get_band_<int64_t>(b);
        case DataType::Unsigned64:
            return get_band_<uint64_t>(b);
    }
}

// Calculate byte position of pixel inside of data file based on interleave
uint64_t ENVI::pos_of_elem_(
    uint64_t band, uint64_t y, uint64_t x, uint64_t size)
{
    switch (interleave_) {
        case Interleave::BandSequential:
            return size * ((samples_ * lines_ * band) + (samples_ * y) + x);
        case Interleave::BandByPixel:
            return size * ((bands_ * samples_ * y) + (bands_ * x) + band);
        case Interleave::BandByLine:
            return size * ((samples_ * bands_ * y) + (samples_ * band) + x);
    }
}

// Print the parsed values of the header
void ENVI::printHeader()
{
    std::cerr << "DataType: " << static_cast<int>(type_) << std::endl;

    std::cerr << "Endianness: " << static_cast<int>(endian_) << std::endl;

    std::cerr << "Interleave: ";
    switch (interleave_) {
        case Interleave::BandSequential:
            std::cerr << "BSQ";
            break;
        case Interleave::BandByPixel:
            std::cerr << "BIP";
            break;
        case Interleave::BandByLine:
            std::cerr << "BIL";
            break;
    }
    std::cerr << std::endl;

    std::cerr << "Samples (Width): " << samples_ << std::endl;
    std::cerr << "Lines (Height): " << lines_ << std::endl;
    std::cerr << "Bands (Depth): " << bands_ << std::endl;
    std::cerr << "Band ID count: " << wavelengths_.size() << std::endl;
}