#include "opencv_gmg.h"
#include <QJsonObject>

//#define DEBUG

constexpr auto INITIALIZATION_FRAMES{ "InitializationFrames" };
constexpr auto DECISION_THRESHOLD{ "DecisionThreshold" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::GMG::GMG(QJsonObject const &a_config)
  : m_initializationFrames{ a_config[INITIALIZATION_FRAMES].toInt() }
  , m_decisionThreshold{ a_config[DECISION_THRESHOLD].toDouble() }
  , m_learningRate{ a_config[LEARNING_RATE].toDouble() }

{
  #ifdef DEBUG
  Logger->debug("Subtractor::GMG() m_initializationFrames{}, m_decisionThreshold:{}", m_initializationFrames, m_decisionThreshold);
  #endif
  m_backgroundSubtractor = cv::bgsegm::createBackgroundSubtractorGMG(m_initializationFrames, m_decisionThreshold);
}
void Subtractors::GMG::process(std::vector<_data> &_data)
{
  cv::Mat out;
  m_backgroundSubtractor->apply(_data[0].processing, out, m_learningRate);
  _data[0].processing = out;
}
