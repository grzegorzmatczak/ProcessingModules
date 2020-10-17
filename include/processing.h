#ifndef PROCESSING_H
#define PROCESSING_H

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

#include "../../ConfigReader/include/configreader.h"

struct _data {
  cv::Mat processing;
};

class Filter;
class Subtractor;
class Adder;

class Processing : public QObject {
  Q_OBJECT

public:

  explicit Processing(QObject *parent = nullptr);

  static Processing *make(QString model);
  virtual void configure(QJsonObject const &a_config) = 0;
  virtual void process(std::vector<_data> &_data) = 0;
  virtual double getElapsedTime() = 0;
};

#endif // PROCESSING_H
