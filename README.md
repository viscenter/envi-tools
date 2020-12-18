# envi-tools

## Requirements
- C++14 compiler
- CMake 3.5+
- Boost
- OpenCV 3+
- Libtiff

## Compilation
```shell
# Get a copy of the source
git clone https://github.com/viscenter/envi-tools.git

# Make a build directory
cd envi-tools/
mkdir build/
cd build/

# Configure and compile
cmake ..
make -j4
```

Compiled applications can be found in `build/bin/`.

## Tools
- `et_convert`: Convert 32-bit floating point TIFFs to 8/16bpc using [linear tone mapping with gamma correction](https://docs.opencv.org/3.4/d6/df5/group__photo__hdr.html#gabcbd653140b93a1fa87ccce94548cd0d).
- `et_convert_dir`: Convert a directory of 32-bit TIFFs to 8/16bpc
- `et_envi_info`: Print metadata from an ENVI header file
- `et_extract`: Extract band images from an ENVI file
- `et_rgb`: Combine 3 single-channel images (assumably RGB) into a single 3-channel image.
