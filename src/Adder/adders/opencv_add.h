#ifndef ADDERS_OPENCV_ADD_H
#define ADDERS_OPENCV_ADD_H

#include "baseadder.h"

class QJsonObject;

namespace Adders
{
class Add : public BaseAdder
{
 public:
  Add(QJsonObject const &a_config);
  void process(std::vector<_data> &_data);

 private:

};
} // namespace Adders

#endif // ADDERS_OPENCV_ADD_H
