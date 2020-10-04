#ifndef BLOCK_H
#define BLOCK_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>
#include <QObject>
#include <QtCore>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

struct _data {
  cv::Mat processing;
};

class Filter;
class Subtractor;
class Adder;

class Block : public QObject {
  Q_OBJECT

public:
  //enum Model { FILTER, ESTIMATOR };
  //Q_ENUM(Model);

  explicit Block(QObject *parent = nullptr);

  static Block *make(QString model);
  virtual void configure(QJsonObject const &a_config) = 0;
  virtual void process(std::vector<_data> &_data) = 0;
  virtual double getElapsedTime() = 0;
};

#endif // BLOCK_H
