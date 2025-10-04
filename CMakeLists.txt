cmake_minimum_required(VERSION 3.10)
project(PresageProject VERSION 0.1.0)

include(CTest)
enable_testing()

set(FFTW_INSTALL_DIR "C:/Program Files/Common Files/fftw3")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${FFTW_INSTALL_DIR})

add_executable(PresageProject main.cpp HeartRateReader.cpp)

target_link_libraries(PresageProject
    ${OpenCV_LIBS}
    ${FFTW_INSTALL_DIR}/libfftw3-3.lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)