#include "opencv_mog.h"
#include <QJsonObject>

//#define DEBUG

constexpr auto HISTORY{ "History" };
constexpr auto N_MIXTURES{ "Nmixtures" };
constexpr auto BACKGROUND_RATIO{ "BackgroundRatio" };
constexpr auto NOISE_SIGMA{ "NoiseSigma" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::MOG::MOG(QJsonObject const &a_config)
  : m_history{ a_config[HISTORY].toInt() }
  , m_nmixtures{ a_config[N_MIXTURES].toInt() }
  , m_backgroundRatio{ a_config[BACKGROUND_RATIO].toDouble() }
  , m_noiseSigma{ a_config[NOISE_SIGMA].toDouble() }
  , m_learningRate{ a_config[LEARNING_RATE].toDouble() }

{
  #ifdef DEBUG
  Logger->debug("Subtractor::MOG() m_backgroundRatio{}, m_noiseSigma:{}", m_backgroundRatio, m_noiseSigma);
  #endif
  m_backgroundSubtractor =
      cv::bgsegm::createBackgroundSubtractorMOG(m_history, m_nmixtures, m_backgroundRatio, m_noiseSigma);
}
void Subtractors::MOG::process(std::vector<_data> &_data)
{
  cv::Mat out;
  m_backgroundSubtractor->apply(_data[0].processing, out, m_learningRate);
  _data[0].processing = out;
}
