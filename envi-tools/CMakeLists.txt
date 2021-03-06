project(libenvitools VERSION 0.1 LANGUAGES CXX)
set(module "envitools")
set(target "${module}")

set(srcs
    src/ENVI.cpp
    src/CSVIO.cpp
    src/TIFFIO.cpp
    src/ContrastMetrics.cpp
)

add_library(${target} ${srcs})
add_library("${namespace}${module}" ALIAS "${target}")
target_include_directories(${target}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${include_install_dir}>
)
target_link_libraries(${target}
    PUBLIC
        Boost::filesystem
        opencv_core
        opencv_photo
    PRIVATE
        TIFF::TIFF
)
set_target_properties(${target} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    EXPORT_NAME "${module}"
)

install(
    TARGETS "${target}"
    EXPORT "${targets_export_name}"
    ARCHIVE DESTINATION "lib"
    LIBRARY DESTINATION "lib"
    INCLUDES DESTINATION "${include_install_dir}/${module}"
    RUNTIME DESTINATION "bin"
)
install(
    DIRECTORY "include/${module}"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING REGEX ".*\.(h|hpp)$"
)
