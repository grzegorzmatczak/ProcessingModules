#ifndef SUBTRACTOR_H
#define SUBTRACTOR_H

#include <opencv2/imgproc.hpp>

#include "../../include/processing.h"

class BaseSubtractor;
class QJsonObject;
namespace cv {
class Mat;
class TickMeter;
} // namespace cv

class Subtractor : public Processing {
  Q_OBJECT
public:
  explicit Subtractor(QObject *parent = nullptr);

  void configure(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);
  double getElapsedTime();

private:
  BaseSubtractor *m_subtractor{};
  cv::TickMeter m_timer;
};

#endif // SUBTRACTOR_H
