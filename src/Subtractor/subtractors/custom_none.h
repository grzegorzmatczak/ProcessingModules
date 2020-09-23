#ifndef SUBTRACTORS_CUSTOM_NONE_H
#define SUBTRACTORS_CUSTOM_NONE_H

#include <opencv2/video.hpp>

#include "basesubtractor.h"

class QJsonObject;

namespace Subtractors {
class None : public BaseSubtractor {
public:
  None();
  void process(std::vector<_data> &_data);

private:
};
} // namespace Subtractors

#endif // SUBTRACTORS_CUSTOM_NONE_H
