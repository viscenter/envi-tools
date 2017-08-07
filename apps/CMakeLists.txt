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

add_executable(et_convert_dir src/ConvertDir.cpp)
target_link_libraries(et_convert_dir
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
    ET::envitools
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_imgcodecs
)

# Install targets
if(INSTALL_APPS)
install(
    TARGETS
        et_contrast
        et_convert
        et_convert_dir
        et_roi_rng
        et_extract
        et_rgb
    RUNTIME DESTINATION bin
    COMPONENT Programs
)
endif()
