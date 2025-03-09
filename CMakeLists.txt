cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(processing_module
  VERSION 0.0.1
  DESCRIPTION "processing_module"
  LANGUAGES C CXX)

set(TARGET_NAME "processing_module")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS " )
endif (WIN32)
 
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -lX11 -Wall" )
endif (UNIX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (WIN32)
    set(QT5_PATH  "D:\\Qt5\\5.15.2\\msvc2019_64")
    list(APPEND CMAKE_PREFIX_PATH ${QT5_PATH})
    set(OpenCV_DIR "D:\\repo\\opencv-4.10.0-build")
    list(APPEND CMAKE_PREFIX_PATH CMAKE_INSTALL_LIBDIR)
    list(APPEND CMAKE_PREFIX_PATH CMAKE_INSTALL_PREFIX)
    
endif (WIN32)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV  REQUIRED)
find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio video tracking)
include_directories(${OpenCV_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


  file(GLOB_RECURSE ADDER_SRC src/Adder/*.cc src/Adder/*.c)
  file(GLOB_RECURSE ADDER_HEADERS src/Adder/*.h src/Adder/*.hpp)

  file(GLOB_RECURSE FILTER_SRC src/Filter/*.cc src/Filter/*.c)
  file(GLOB_RECURSE FILTER_HEADERS src/Filter/*.h src/Filter/*.hpp)

  file(GLOB_RECURSE SUBTRACTOR_SRC src/Subtractor/*.cc src/Subtractor/*.c)
  file(GLOB_RECURSE SUBTRACTOR_HEADERS src/Subtractor/*.h src/Subtractor/*.hpp)

set(SRC
   src/processing.cc
)

set(HEADERS
  include/processing.h
)

set(SRC_HEADERS_TEST
	gtest/gtest.cc
    gtest/gtest_filter_custom_addmultipledron.cc
    gtest/gtest_filter_custom_addmultipledron.h
    gtest/gtest_filter_custom_velocityfilter.cc
    gtest/gtest_filter_custom_velocityfilter.h
    gtest/gtest_filter_custom_viterbi.cc
    gtest/gtest_filter_custom_viterbi.h
    gtest/gtest_filter_median_impl_2.cc
    gtest/gtest_filter_median_impl_2.h
)

include_directories( "src" )
include_directories( "include" )
include_directories( "src/Subtractor" )
include_directories( "src/Subtractor/subtractors" )
include_directories( "src/Filter" )
include_directories( "src/Filter/filters" )
include_directories( "src/Adder" )
include_directories( "src/Adder/adders" )

add_library(${PROJECT_NAME}_lib STATIC
    ${SRC} ${HEADERS}
    ${ADDER_SRC}  ${ADDER_HEADERS}
    ${FILTER_SRC} ${FILTER_HEADERS}
    ${SUBTRACTOR_SRC} ${SUBTRACTOR_HEADERS}
)

target_link_libraries(${PROJECT_NAME}_lib utils2_lib Qt::Core Threads::Threads ${OpenCV_LIBS})

install(TARGETS ${PROJECT_NAME}_lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    
install(TARGETS ${PROJECT_NAME}_lib EXPORT ${PROJECT_NAME}Config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

MESSAGE(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")
MESSAGE(STATUS "CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")

add_executable(${PROJECT_NAME}_test ${SRC_HEADERS_TEST})
target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib gtest gmock)

#export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)