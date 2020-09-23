#ifndef ADDERS_OPENCV_ADD_WEIGHTED_H
#define ADDERS_OPENCV_ADD_WEIGHTED_H

#include "baseadder.h"

class QJsonObject;

namespace Adders
{
class AddWeighted : public BaseAdder
{
 public:
  AddWeighted(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

 private:
  double  m_alpha;
  double m_beta;
  double m_gamma;
};
} // namespace Adders

#endif // ADDERS_OPENCV_ADD_WEIGHTED_H
