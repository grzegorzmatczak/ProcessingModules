#ifndef TRACKERS_CUSTOM_NONE_H
#define TRACKERS_CUSTOM_NONE_H

#include "basetracker.h"

class QJsonObject;

namespace Trackers {
class None : public BaseTracker {
public:
  None();

  void process(std::vector<_data> &_data);

private:
};
} // namespace Trackers

#endif // TRACKERS_CUSTOM_NONE_H
