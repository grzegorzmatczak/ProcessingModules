#ifndef CUSTOM_VITERBI_H
#define CUSTOM_VITERBI_H

#include "basefilter.h"
#include <deque>

class QJsonObject;

namespace Filters {
class Viterbi : public BaseFilter {
 public:
  Viterbi(QJsonObject const &a_config);

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

#endif // CUSTOM_VITERBI_H
