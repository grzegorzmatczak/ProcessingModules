#include "opencv_mog2.h"
#include <QJsonObject>

constexpr auto HISTORY{ "History" };
constexpr auto VAR_THRESHOLD{ "VarThreshold" };
constexpr auto DETECT_SHADOW{ "DetectShadow" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::MOG2::MOG2(QJsonObject const &a_config)
  : m_history{ a_config[HISTORY].toInt() }
  , m_varThreshold{ a_config[VAR_THRESHOLD].toDouble() }
  , m_detectShadows{ a_config[DETECT_SHADOW].toBool() }
  , m_learningRate{ a_config[LEARNING_RATE].toDouble() }
{
  m_backgroundSubtractor = cv::createBackgroundSubtractorMOG2(m_history, m_varThreshold, m_detectShadows);
}
void Subtractors::MOG2::process(std::vector<_data> &_data)
{
  cv::Mat _out;
  m_backgroundSubtractor->apply(_data[0].processing, _out, m_learningRate);
  _data[0].processing = _out;
}
