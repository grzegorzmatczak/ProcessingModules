#ifndef FILTERS_OPENCV_CANNY_H
#define FILTERS_OPENCV_CANNY_H

#include "basefilter.h"

class QJsonObject;

/* Paste in configuration:
[{
  "name": "Canny_Threshold1",
  "min": 1,
  "max": 2
},{
  "name": "Canny_Threshold2",
  "min": 1,
  "max": 2
},{
  "name": "Canny_ApertureSize",
  "min": 1,
  "max": 7,
  "isOdd": true
},{
  "name": "Canny_L2Gradient",
  "min": 1,
  "max": 2
}],
*/

namespace Filters {
class Canny : public BaseFilter {
public:
  Canny(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  double m_threshold1{};
  double m_threshold2{};
  int m_apertureSize{};
  bool m_L2Gradient{};
};
} // namespace Filters

#endif // FILTERS_OPENCV_CANNY_H
