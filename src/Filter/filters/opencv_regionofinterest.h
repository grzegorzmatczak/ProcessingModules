#ifndef FILTER_REGION_OF_INTEREST_H
#define FILTER_REGION_OF_INTEREST_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class RegionOfInterest : public BaseFilter
{
 public:
  RegionOfInterest(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int const m_width;
  int const m_height;
  int const m_x;
  int const m_y;
};
} // namespace Filters

#endif // FILTER_REGION_OF_INTEREST_H
