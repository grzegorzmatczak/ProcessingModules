#ifndef CUSTOM_VELOCITY_FILTER_H
#define CUSTOM_VELOCITY_FILTER_H

#include "basefilter.h"
#include <deque>

class QJsonObject;

namespace Filters {
class VelocityFilter : public BaseFilter {
 public:
  VelocityFilter(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  private:
  QRandomGenerator *m_randomGenerator;
  std::deque<cv::Mat> m_images;
  std::deque<cv::Mat> m_VAL;

  bool m_firstTime;
  int m_width;
  int m_height;
};
} // namespace Filters

#endif // CUSTOM_VELOCITY_FILTER_H
