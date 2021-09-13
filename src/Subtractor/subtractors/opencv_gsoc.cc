#include "opencv_gsoc.h"
#include <QJsonObject>

constexpr auto CAMERA_MOTION_COMPENSATION{ "CameraMotionCompensation" };
constexpr auto NUMBER_OF_SAMPLES{ "NumberOfSamples" };
constexpr auto REPLACE_RATE{ "ReplaceRate" };
constexpr auto PROPAGATION_RATE{ "PropagationRate" };
constexpr auto HITS_THRESHOLD{ "HitsThreshold" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::GSOC::GSOC(QJsonObject const &a_config)
  : m_cameraMotionCompensation{ a_config[CAMERA_MOTION_COMPENSATION].toInt() }
  , m_numberOfSamples{ a_config[NUMBER_OF_SAMPLES].toInt() }
  , m_replaceRate{ a_config[REPLACE_RATE].toDouble() }
  , m_propagationRate{ a_config[PROPAGATION_RATE].toDouble() }
  , m_hitsThreshold{ a_config[HITS_THRESHOLD].toInt() }
  , m_learningRate{ a_config[LEARNING_RATE].toDouble() }

{
  m_backgroundSubtractor = cv::bgsegm::createBackgroundSubtractorGSOC(
      m_cameraMotionCompensation, m_numberOfSamples, m_replaceRate, m_propagationRate, m_hitsThreshold);
}
void Subtractors::GSOC::process(std::vector<_data> &_data)
{
  cv::Mat out;
  m_backgroundSubtractor->apply(_data[0].processing, out, m_learningRate);
  _data[0].processing = out;
}
