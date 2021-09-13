#ifndef SUBTRACTORS_OPENCV_GSOC_H
#define SUBTRACTORS_OPENCV_GSOC_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class GSOC : public BaseSubtractor {
public:
  GSOC(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

private:
  cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
  cv::Mat m_background;
  int m_cameraMotionCompensation;
  int m_numberOfSamples;
  double m_replaceRate;
  double m_propagationRate;
  int m_hitsThreshold;
  double m_learningRate;
};
} // namespace Subtractors

#endif // SUBTRACTORS_OPENCV_GSOC_H
