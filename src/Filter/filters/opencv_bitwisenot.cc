#include "opencv_bitwisenot.h"

#include <QJsonObject>

//#define DEBUG

Filters::BitwiseNot::BitwiseNot(QJsonObject const &a_config)
{
	#ifdef DEBUG
	Logger->debug("VelocityFilter::VelocityFilter()");
	#endif
}

void Filters::BitwiseNot::process(std::vector<_data> &_data)
{
	//cv::cvtColor(_data[0].processing, _data[0].processing, m_colorCode);
	cv::bitwise_not(_data[0].processing, _data[0].processing);
}
