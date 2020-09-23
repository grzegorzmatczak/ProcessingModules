#ifndef COMPARE_H
#define COMPARE_H

#include <QDebug>
#include <QObject>

#include <opencv2/imgproc.hpp>

#include "../../include/block.h"

class BaseCompare;
class QJsonObject;

class Compare : public Block {
  Q_OBJECT
public:
  explicit Compare(QObject *parent = nullptr);
  void configure(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);
  double getElapsedTime();

private:
  BaseCompare *m_baseCompare{};
  cv::TickMeter m_timer;
};

#endif // COMPARE_H
