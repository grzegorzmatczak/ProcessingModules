#ifndef ADDERS_OPENCV_ABSDIFF_H
#define ADDERS_OPENCV_ABSDIFF_H

#include "baseadder.h"

class QJsonObject;

namespace Adders
{
class AbsDiff : public BaseAdder
{
 public:
  AbsDiff();
  void process(std::vector<_data> &_data);

 private:

};
} // namespace Adders

#endif // ADDERS_OPENCV_ABSDIFF_H
