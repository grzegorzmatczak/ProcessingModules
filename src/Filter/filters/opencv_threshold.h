#ifndef THRESHOLD_H
#define THRESHOLD_H

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
class Threshold : public BaseFilter
{
 public:
  Threshold(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int m_decisionThreshold;
  int m_thresholdTypes;
  int m_thresholdValue;
};
} // namespace Filters

#endif // THRESHOLD_H
