#ifndef ADDER_H
#define ADDER_H

#include <opencv2/imgproc.hpp>

#include "../../include/processing.h"

class BaseAdder;
class QJsonObject;

namespace cv {
class Mat;
class TickMeter;
} // namespace cv

class Adder : public Processing {
  Q_OBJECT
public:
  explicit Adder(QObject *parent = nullptr);

  void configure(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);
  double getElapsedTime();

private:
  BaseAdder *m_adder{};
  cv::TickMeter m_timer;
};

#endif // ADDER_H
