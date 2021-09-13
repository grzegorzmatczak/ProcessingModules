#ifndef SUBTRACTORS_OPENCV_KNN_H
#define SUBTRACTORS_OPENCV_KNN_H

#include <opencv2/video.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class KNN : public BaseSubtractor {
public:
  KNN(QJsonObject const &a_config);
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

#endif // SUBTRACTORS_OPENCV_KNN_H
