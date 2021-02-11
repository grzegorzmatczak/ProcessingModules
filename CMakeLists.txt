cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(ProcessingModules
  VERSION 0.0.1
  DESCRIPTION "ProcessingModules"
  LANGUAGES C CXX)

set(TARGET_NAME "ProcessingModules")
set(DEFAULT_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON} -O1")

set(CMAKE_AUTOMOC ON)

cmake_policy(SET CMP0054 NEW)

if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\opencvBuild")
	list(APPEND CMAKE_PREFIX_PATH "E:\\Qt\\5.12.8\\msvc2017_64")
    list(APPEND CMAKE_PREFIX_PATH "E:\\repo\\GeneticOptimization/ConfigReader")
    
endif (WIN32)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(IncludeSpdlog)
find_package(ConfigReader)

find_package(Threads REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV  REQUIRED)
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

add_library(${PROJECT_NAME} STATIC
    ${SRC} ${HEADERS}
    ${ADDER_SRC}  ${ADDER_HEADERS}
    ${FILTER_SRC} ${FILTER_HEADERS}
    ${SUBTRACTOR_SRC} ${SUBTRACTOR_HEADERS}
)


qt5_use_modules(${PROJECT_NAME} Widgets)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(${PROJECT_NAME} IncludeSpdlog ConfigReader)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network  Threads::Threads)