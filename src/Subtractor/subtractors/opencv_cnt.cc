#include "opencv_cnt.h"
#include <QJsonObject>

constexpr auto MIN_PIXEL_STABILITY{ "MinPixelStability" };
constexpr auto MAX_PIXEL_STABILITY{ "MaxPixelStability" };
constexpr auto IS_PARALLEL{ "IsParallel" };
constexpr auto USE_HISTORY{ "UseHistory" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::CNT::CNT(QJsonObject const &a_config)
  : m_minPixelStability{ a_config[MIN_PIXEL_STABILITY].toInt() }
  , m_maxPixelStability{ a_config[MAX_PIXEL_STABILITY].toInt() }
  , m_isParallel{ a_config[IS_PARALLEL].toBool() }
  , m_useHistory{ a_config[USE_HISTORY].toBool() }
  , m_learningRate{ a_config[LEARNING_RATE].toDouble() }
{
  m_backgroundSubtractor =
      cv::bgsegm::createBackgroundSubtractorCNT(m_minPixelStability, m_useHistory, m_maxPixelStability, m_isParallel);
}

void Subtractors::CNT::process(std::vector<_data> &_data)
{
  cv::Mat out;
  m_backgroundSubtractor->apply(_data[0].processing, out, m_learningRate);
  _data[0].processing = out;
}
