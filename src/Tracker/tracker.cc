#include "tracker.h"
#include "trackerlist.h"

Tracker::Tracker(QObject *parent) : Block(parent) {
  m_baseTracker = new Trackers::None{};
}

void Tracker::configure(QJsonObject const &a_config) {
  //H_logger->trace("Tracker::configure()");
  auto const NAME_STRING{a_config[NAME].toString()};
  //H_logger->trace("Tracker type: {}", NAME_STRING.toStdString());
  delete m_baseTracker;
  m_timer.reset();

  if (NAME_STRING == "OpenCVTrackers") {
    m_baseTracker = new Trackers::Trackers{a_config};
  } else if (NAME_STRING == "None") {
    m_baseTracker = {new Trackers::None{}};
  } else {
    //H_logger->error("Unsupported filter type: {}", NAME_STRING.toStdString());
  }
}

void Tracker::process(std::vector<_data> &_data) {
  //H_logger->trace("Tracker::process(a_image)");
  m_timer.start();
  m_baseTracker->process(_data);
  m_timer.stop();
}
double Tracker::getElapsedTime() { return m_timer.getTimeMilli(); }
