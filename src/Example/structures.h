#ifndef STRUCTURES_PREPROCESSING_MODULES_H
#define STRUCTURES_PREPROCESSING_MODULES_H

#include <stdio.h>
#include <QtCore>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "../../../IncludeSpdlog/spdlog.h"
#include "../../../ConfigReader/src/configreader.h"

constexpr auto GENERAL{ "General" };
constexpr auto GENETIC{ "Genetic" };
constexpr auto GRAPH{ "Graph" };
constexpr auto NAME{ "Name" };
constexpr auto ACTIVE{ "Active" };
constexpr auto TYPE{ "Type" };
constexpr auto NEXT{ "Next" };
constexpr auto PREV{ "Prev" };
constexpr auto CONFIG{ "Config" };

struct _data
{
  cv::Mat input;
  cv::Mat processing;
  qint32 test;
};

struct _list
{
  std::vector<qint32> prev;
  std::vector<_data> active;
  std::vector<qint32> next;
};

#endif // STRUCTURES_PREPROCESSING_MODULES_H
