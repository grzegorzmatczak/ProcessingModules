#ifndef SUBTRACTORS_OPENCV_GMG_H
#define SUBTRACTORS_OPENCV_GMG_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class GMG : public BaseSubtractor {
public:
  GMG(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

private:
  cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
  cv::Mat m_background;
  int m_initializationFrames;
  double m_decisionThreshold;
  double m_learningRate;
};
} // namespace Subtractors

#endif // SUBTRACTORS_OPENCV_GMG_H
