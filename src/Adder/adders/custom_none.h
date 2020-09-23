#ifndef ADDERS_NONE_H
#define ADDERS_NONE_H

#include "baseadder.h"

class QJsonObject;

namespace Adders
{
class None : public BaseAdder
{
 public:
  None();

  void process(std::vector<_data> &_data);

 private:
};
} // namespace Adders

#endif // ADDERS_NONE_H
