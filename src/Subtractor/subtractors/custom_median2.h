#ifndef SUBTRACTORS_CUSTOM_MEDIAN_2_H
#define SUBTRACTORS_CUSTOM_MEDIAN_2_H

#include <opencv2/bgsegm.hpp>

#include "basesubtractor.h"
#include "custom_median_implementation_2.h"

class QJsonObject;

namespace Subtractors {
class Median2 : public BaseSubtractor {
public:
  Median2(QJsonObject const &a_config);
  ~Median2();
  void process(std::vector<_data> &_data);
  cv::Mat getDiff(cv::Mat &a_src);
  cv::Mat getBackground();

private:
  Subtractors::MedianImage* m_backgroundSubtractor;
  cv::Mat m_background;

  int m_history;
  int m_historyDelta;
  int m_iteration;
  bool m_firstInitialization;
  bool m_firstTime;
  int m_decisionThreshold;
};
} // namespace Subtractors

#endif // SUBTRACTORS_CUSTOM_MEDIAN_2_H
