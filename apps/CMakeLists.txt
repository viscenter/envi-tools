add_executable(et_contrast src/CalculateContrast.cpp)
target_link_libraries(et_contrast
    ET::envitools
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_highgui
    opencv_imgcodecs
    opencv_imgproc
)

add_executable(et_convert src/Convert.cpp)
target_link_libraries(et_convert
    ET::envitools
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_imgcodecs
    opencv_photo
)

add_executable(et_roi_rng src/RandomPointGen.cpp)
target_link_libraries(et_roi_rng
    ET::envitools
    Boost::filesystem
    Boost::program_options
)

add_executable(et_extract src/ExtractENVI.cpp)
target_link_libraries(et_extract
    ET::envitools
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_imgcodecs
)

add_executable(et_rgb src/MakeRGB.cpp)
target_link_libraries(et_rgb
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_imgcodecs
)
