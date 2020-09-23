#ifndef MEDIANBLUR_H
#define MEDIANBLUR_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class MedianBlur : public BaseFilter
{
 public:
  MedianBlur(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int const m_kernelSize;
};
} // namespace Filters

#endif // MEDIANBLUR_H
