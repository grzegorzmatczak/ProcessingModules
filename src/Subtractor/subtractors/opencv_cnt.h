#ifndef SUBTRACTORS_OPENCV_CNT_H
#define SUBTRACTORS_OPENCV_CNT_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class CNT : public BaseSubtractor {
public:
  CNT(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

private:
  cv::Ptr<cv::BackgroundSubtractor> m_backgroundSubtractor;
  cv::Mat m_background;
  int m_minPixelStability;
  int m_maxPixelStability;
  bool m_isParallel;
  bool m_useHistory;
  double m_learningRate;
};
} // namespace Subtractors

#endif // SUBTRACTORS_OPENCV_CNT_H
