#ifndef MORPHOLOGY_OPERATION_H
#define MORPHOLOGY_OPERATION_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class MorphologyOperation : public BaseFilter
{
 public:
  MorphologyOperation(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  cv::Mat const m_Element;
  int const m_morphOperator;
  int const m_morphSize;
  int const m_morphElement;
};
} // namespace Filters

#endif // MORPHOLOGY_OPERATION_H
