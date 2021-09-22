#include "opencv_knn.h"
#include <QJsonObject>

//#define DEBUG

constexpr auto HISTORY{ "History" };
constexpr auto VAR_THRESHOLD{ "VarThreshold" };
constexpr auto DETECT_SHADOW{ "DetectShadow" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::KNN::KNN(QJsonObject const &a_config)
	: m_history{ a_config[HISTORY].toInt() }
	, m_varThreshold{ a_config[VAR_THRESHOLD].toDouble() }
	, m_detectShadows{ a_config[DETECT_SHADOW].toBool() }
	, m_learningRate{ a_config[LEARNING_RATE].toDouble() }
{
	#ifdef DEBUG
	Logger->debug("Subtractors::KNN::KNN()");
	Logger->debug("m_learningRate:{}", m_learningRate);
	#endif

	m_backgroundSubtractor = cv::createBackgroundSubtractorKNN(m_history, m_varThreshold, m_detectShadows);
}
void Subtractors::KNN::process(std::vector<_data> &_data)
{
	cv::Mat out;
	m_backgroundSubtractor->apply(_data[0].processing, out, m_learningRate);
	_data[0].processing = out;
}
