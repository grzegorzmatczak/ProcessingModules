#ifndef BLOCK_H
#define BLOCK_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>
#include <QPolygonF>
#include <QtCore>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

constexpr auto GRAPH{"Graph"};
constexpr auto NAME{"Name"};
constexpr auto ACTIVE{"Active"};
constexpr auto COPY_SIGNAL{ "Signal" };
constexpr auto TYPE{"Type"};
constexpr auto NEXT{"Next"};
constexpr auto PREV{"Prev"};
constexpr auto CONFIG{"Config"};

constexpr auto TRACKERS{"Trackers"};
constexpr auto WIDTH{"Width"};
constexpr auto HEIGHT{"Height"};
constexpr auto INIT_FRAME{"InitFrame"};

constexpr auto COMPARE{"COMPARE"};
constexpr auto VOT_CPP{"VOTCpp"};
constexpr auto GROUNDTRUTH{"GroundtruthString"};

constexpr auto X{ "X" };
constexpr auto Y{ "Y" };


typedef struct regionBounds {
  float top;
  float bottom;
  float left;
  float right;
} regionBounds;

typedef struct regionRectangle {
  float x;
  float y;
  float width;
  float height;
} regionRectangle;

typedef struct regionMask {
  float x;
  float y;
  float width;
  float height;
  char *data;
} regionMask;

typedef enum regionType { EMPTY, SPECIAL, RECTANGLE, POLYGON, MASK } regionType;
typedef struct regionContainer {
  enum regionType type;
  union {
    regionRectangle rectangle;
    regionMask mask;
  } data;

} regionContainer;

typedef struct regionOverlap {
  float overlap;
  float only1;
  float only2;
} regionOverlap;

struct _data {
  cv::Mat input;
  cv::Mat processing;
  qint32 test;
  std::vector<QPolygonF> bounds;
  std::vector<cv::Rect> rects;
  QString nameOfTracker;
  QString nameOfFile;
  QString testStr;
};

struct _list {
  std::vector<qint32> prev;
  std::vector<_data> active;
  std::vector<qint32> next;
};

class Filter;
class Subtractor;
class Adder;
class Tracker;

class Block : public QObject {
  Q_OBJECT

public:
  enum Model { FILTER, ESTIMATOR };
  Q_ENUM(Model);

  explicit Block(QObject *parent = nullptr);

  static Block *make(QString model);
  virtual void configure(QJsonObject const &a_config) = 0;
  virtual void process(std::vector<_data> &_data) = 0;
  virtual double getElapsedTime() = 0;
};

#endif // BLOCK_H
