#ifndef FILTERS_CUSTOM_NONE_H
#define FILTERS_CUSTOM_NONE_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class None : public BaseFilter
{
 public:
  None();

  void process(std::vector<_data> &_data);

 private:
};
} // namespace Filters

#endif // FILTERS_CUSTOM_NONE_H
