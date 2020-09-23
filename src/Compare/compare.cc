#include "compare.h"
#include "comparelist.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(CompareLogger, "Compare")

Compare::Compare(QObject *parent) : Block(parent) {
  m_baseCompare = new Compares::None{};
  qCInfo(CompareLogger) << "Compare()";
}

void Compare::configure(QJsonObject const &a_config) {
  auto const NAME_STRING{a_config[NAME].toString()};
  qCInfo(CompareLogger) << "configure() Compare type:" << NAME_STRING;
  delete m_baseCompare;
  m_timer.reset();

  if (NAME_STRING == "None") {
    m_baseCompare = new Compares::None{};
  } else if (NAME_STRING == "VOT") {
    m_baseCompare = new Compares::VOT{a_config};
  } else if (NAME_STRING == "VOTCpp") {
    m_baseCompare = new Compares::VOTCpp{a_config};
  } else {
    qCWarning(CompareLogger)
        << "configure() Unsupported filter type:" << NAME_STRING;
  }
}

void Compare::process(std::vector<_data> &_data) {
  qCDebug(CompareLogger) << "configure() process";
  m_timer.start();
  m_baseCompare->process(_data);
  m_timer.stop();
}
double Compare::getElapsedTime() { return m_timer.getTimeMilli(); }
