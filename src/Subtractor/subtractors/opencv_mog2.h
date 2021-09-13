#ifndef SUBTRACTORS_OPENCV_MOG2_H
#define SUBTRACTORS_OPENCV_MOG2_H

#include "basesubtractor.h"
#include <opencv2/video.hpp>

class QJsonObject;

namespace Subtractors {
class MOG2 : public BaseSubtractor {
public:
  MOG2(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

private:
  cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
  cv::Mat m_background;
  int m_history;
  double m_varThreshold;
  bool m_detectShadows;
  double m_learningRate;
};
} // namespace Subtractors

#endif // SUBTRACTORS_OPENCV_MOG2_H
