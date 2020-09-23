#ifndef COLOR_H
#define COLOR_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class Color : public BaseFilter
{
 public:
  Color(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int const m_colorCode;
};
} // namespace Filters

#endif // COLOR_H
