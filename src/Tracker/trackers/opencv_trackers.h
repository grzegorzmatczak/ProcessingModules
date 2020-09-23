#ifndef TRACKERS_OPENCV_TRACKERS_H
#define TRACKERS_OPENCV_TRACKERS_H

#include "basetracker.h"

#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>

class QJsonObject;

namespace Trackers {
class Trackers : public BaseTracker {
public:
  Trackers(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  std::vector<cv::Ptr<cv::Tracker>> m_trackers;
  QJsonArray m_configTracker;
  std::vector<bool> m_activeTracking;
  std::vector<qint32> m_frameTracking;
  qint32 m_frameIteration{};
};
} // namespace Trackers

#endif // TRACKERS_OPENCV_TRACKERS_H
