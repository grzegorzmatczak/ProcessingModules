#ifndef SUBTRACTORS_OPENCV_MOG_H
#define SUBTRACTORS_OPENCV_MOG_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class MOG : public BaseSubtractor {
public:
  MOG(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

private:
  cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
  cv::Mat m_background;
  int m_history;
  int m_nmixtures;
  double m_backgroundRatio;
  double m_noiseSigma;
  double m_learningRate;
};
} // namespace Subtractors

#endif // SUBTRACTORS_OPENCV_MOG_H
