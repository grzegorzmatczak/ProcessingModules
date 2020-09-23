#ifndef COMPARES_VOTCPP_CHANLLENGE_H
#define COMPARES_VOTCPP_CHANLLENGE_H

#include "basecompare.h"

class QJsonObject;

namespace Compares {
class VOTCpp : public BaseCompare {
public:
  VOTCpp(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  QStringList m_groundTruth;
  std::vector<QPolygonF> m_rect;
  qint32 m_counter;
};
} // namespace Compares

#endif // COMPARES_VOTCPP_CHANLLENGE_H
