#include "custom_median2.h"
#include <QJsonObject>

//#define DEBUG

constexpr auto HISTORY{"History"};
constexpr auto HISTORYDELTA{"HistoryDelta"};
constexpr auto DECISION_THRESHOLD{"DecisionThreshold"};

Subtractors::Median2::Median2(QJsonObject const &a_config) : 
	m_history{a_config[HISTORY].toInt()},
	m_historyDelta{a_config[HISTORYDELTA].toInt()}, 
	m_iteration(0),
	m_firstInitialization(true),
	m_firstTime(true), 
	m_decisionThreshold{ a_config[DECISION_THRESHOLD].toInt()}
{
	#ifdef DEBUG
	Logger->debug("Subtractors::Median2::Median2()");
	#endif
	m_backgroundSubtractor = new Subtractors::MedianImage(m_history);
}

Subtractors::Median2::~Median2()
{
	delete m_backgroundSubtractor;
}

void Subtractors::Median2::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::Median2::process()");
	#endif
	m_iteration++;
	if (m_firstTime)
	{
		m_firstTime = false;
		m_background = _data[0].processing.clone();
		m_backgroundSubtractor->initMedian(_data[0].processing);
		m_backgroundSubtractor->getMedianImage(m_background);

	} 
	else if ((m_firstInitialization) && (m_iteration < m_historyDelta))
	{
		m_backgroundSubtractor->addImage(_data[0].processing);
	}
	else if ((m_firstInitialization) && (m_iteration >= m_historyDelta))
	{
		m_firstInitialization = false;
	}
	else if (m_iteration >= m_historyDelta) {
		m_backgroundSubtractor->addImage(_data[0].processing);
		m_backgroundSubtractor->getMedianImage(m_background);
		m_iteration = 0;
	}
	cv::Mat frame_diff;
	cv::absdiff(_data[0].processing, m_background, frame_diff);
	cv::threshold(frame_diff, _data[0].processing, m_decisionThreshold, 255, 0);
}

cv::Mat Subtractors::Median2::getDiff(cv::Mat &a_src)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::Median2::getDiff()");
	#endif
	cv::Mat frame_diff;
	cv::absdiff(a_src, m_background, frame_diff);
	cv::threshold(frame_diff, frame_diff, m_decisionThreshold, 255, 0);
	return frame_diff;
}

cv::Mat Subtractors::Median2::getBackground()
{ 
	#ifdef DEBUG
	Logger->debug("Subtractors::Median2::getBackground()");
	#endif
	return m_background; 
}
