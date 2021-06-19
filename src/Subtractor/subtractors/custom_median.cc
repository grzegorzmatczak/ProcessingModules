#include "custom_median.h"
#include <QJsonObject>

constexpr auto HISTORY{"History"};
constexpr auto HISTORYDELTA{"HistoryDelta"};
constexpr auto DECISION_THRESHOLD{"DecisionThreshold"};

Subtractors::Median::Median(QJsonObject const &a_config) : 
	m_history{a_config[HISTORY].toInt()},
	m_historyDelta{a_config[HISTORYDELTA].toInt()}, 
	m_iteration(0),
	m_firstInitialization(true),
	m_firstTime(true), 
	m_decisionThreshold{ a_config[DECISION_THRESHOLD].toInt()}
{
	m_backgroundSubtractor =Subtractors::imgMedian(static_cast<unsigned char>(m_history));
}
void Subtractors::Median::process(std::vector<_data> &_data)
{
	m_iteration++;
	if (m_firstTime)
	{
		m_firstTime = false;
		m_backgroundSubtractor.InitMedian(_data[0].processing);
		m_backgroundSubtractor.RemoveNode();
		m_backgroundSubtractor.AddImage(_data[0].processing);
		m_background = m_backgroundSubtractor.getMedian();
	} 
	else if ((m_firstInitialization) && (m_iteration < m_historyDelta))
	{
		m_backgroundSubtractor.RemoveNode();
		m_backgroundSubtractor.AddImage(_data[0].processing);
	}
	else if ((m_firstInitialization) && (m_iteration >= m_historyDelta))
	{
		m_firstInitialization = false;
	}
	else if (m_iteration >= m_historyDelta) {
		m_backgroundSubtractor.RemoveNode();
		m_backgroundSubtractor.AddImage(_data[0].processing);
		m_background = m_backgroundSubtractor.getMedian();
		m_iteration = 0;
	}
	cv::Mat frame_diff;
	cv::absdiff(_data[0].processing, m_background, frame_diff);
	cv::threshold(frame_diff, _data[0].processing, m_decisionThreshold, 255, 0);
}

cv::Mat Subtractors::Median::getDiff(cv::Mat &a_src)
{
	cv::Mat frame_diff;
	cv::absdiff(a_src, m_background, frame_diff);
	cv::threshold(frame_diff, frame_diff, m_decisionThreshold, 255, 0);
	return frame_diff;
}

cv::Mat Subtractors::Median::getBackground()
{ 
	return m_background; 
}
