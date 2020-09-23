#ifndef RESIZE_H
#define RESIZE_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class Resize : public BaseFilter
{
 public:
  Resize(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int const m_width;
  int const m_height;
};
} // namespace Filters

#endif // RESIZE_H
