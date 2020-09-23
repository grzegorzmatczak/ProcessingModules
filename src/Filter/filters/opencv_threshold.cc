#include "opencv_threshold.h"
#include <QJsonObject>

constexpr auto THRESHOLD_DECISION{ "ThresholdDecision" };
constexpr auto THRESHOLD_TYPES{ "ThresholdTypes" };
constexpr auto THRESHOLD_VALUE{ "ThresholdValue" };

Filters::Threshold::Threshold(QJsonObject const &a_config)
  : m_decisionThreshold{ a_config[THRESHOLD_DECISION].toInt() }
  , m_thresholdTypes{ a_config[THRESHOLD_TYPES].toInt() }
  , m_thresholdValue{ a_config[THRESHOLD_VALUE].toInt() }
{
}

void Filters::Threshold::process(std::vector<_data> &_data)
{
  cv::threshold(_data[0].processing, _data[0].processing, m_decisionThreshold, m_thresholdValue, m_thresholdTypes);
}
