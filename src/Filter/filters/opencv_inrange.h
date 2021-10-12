#ifndef FILTERS_OPENCV_IN_RANGE_H
#define FILTERS_OPENCV_IN_RANGE_H

#include "basefilter.h"

class QJsonObject;

/* Paste in configuration:
[{
  "name": "Sobel_Lowerb1",
  "min": 1,
  "max": 2
}]

{
        "Name": "E",
        "Active": 1,
        "Type": "Filter",
        "Next":[{"Name": "F", "Active": 2}],
        "Prev":[{"Name": "D", "Active": 0}],
        "Config":{"Name": "InRange", "Lowerb1": 100,"Lowerb2":100, "Lowerb3":
100, "Upperb1": 255, "Upperb2": 255, "Upperb3": 255}
}

*/

namespace Filters {
class InRange : public BaseFilter {
public:
  InRange(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  int m_lowerb1;
  int m_lowerb2;
  int m_lowerb3;

  int m_upperb1;
  int m_upperb2;
  int m_upperb3;
};
} // namespace Filters

#endif // FILTERS_OPENCV_IN_RANGE_H
