add_executable(et_contrast src/contrast.cpp)
target_link_libraries(et_contrast
	ET::envitools
        Boost::filesystem
        opencv_core
        opencv_highgui
        opencv_imgcodecs
        opencv_imgproc
        opencv_photo
        )

add_executable(et_convert src/convert.cpp)
target_link_libraries(et_convert
        Boost::filesystem
        opencv_core
        opencv_imgcodecs
        opencv_photo
        )

add_executable(et_PointGenerator src/randomPointGenerator.cpp)
target_link_libraries(et_PointGenerator
    ET::envitools
    Boost::filesystem
)

add_executable(et_ReadENVI src/ReadENVI.cpp)
target_link_libraries(et_ReadENVI
        ET::envitools
        opencv_core
        opencv_imgcodecs
        )
