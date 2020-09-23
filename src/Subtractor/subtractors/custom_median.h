#ifndef SUBTRACTORS_CUSTOM_MEDIAN_H
#define SUBTRACTORS_CUSTOM_MEDIAN_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"
#include "custom_median_implementation.h"

class QJsonObject;

namespace Subtractors {
class Median : public BaseSubtractor {
public:
  Median(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);
  cv::Mat getDiff(cv::Mat &a_src);
  cv::Mat getBackground();

private:
  Subtractors::imgMedian m_backgroundSubtractor;
  cv::Mat m_background;

  int m_history;
  int m_historyDelta;
  int m_iteration;
  bool m_firstInitialization;
  bool m_firstTime;
  int m_decisionThreshold;
};
} // namespace Subtractors

#endif // SUBTRACTORS_CUSTOM_MEDIAN_H
