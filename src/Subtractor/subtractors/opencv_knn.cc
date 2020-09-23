#include "opencv_knn.h"
#include <QJsonObject>
//#include <opencv2/tracking/tracker.hpp>

constexpr auto LEARNING_RATE{ "LearningRate" };
constexpr auto HISTORY{ "History" };
constexpr auto VAR_THRESHOLD{ "VarThreshold" };
constexpr auto DETECT_SHADOW{ "DetectShadow" };

Subtractors::KNN::KNN(QJsonObject const &a_config)
  : m_learningRate{ a_config[LEARNING_RATE].toDouble() }
  , m_history{ a_config[HISTORY].toInt() }
  , m_varThreshold{ a_config[VAR_THRESHOLD].toDouble() }
  , m_detectShadows{ a_config[DETECT_SHADOW].toBool() }
{
  //H_logger->trace("Subtractors::KNN::KNN()");
  m_backgroundSubtractor = cv::createBackgroundSubtractorKNN(m_history, m_varThreshold, m_detectShadows);
}
void Subtractors::KNN::process(std::vector<_data> &_data)
{
  //H_logger->trace("Subtractors::KNN::process()");
  cv::Mat out;
  m_backgroundSubtractor->apply(_data[0].processing, out);
  _data[0].processing = out;
  //H_logger->trace("Subtractors::KNN::process() done");
}
