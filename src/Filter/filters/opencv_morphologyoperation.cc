#include "opencv_morphologyoperation.h"
#include <QJsonObject>

//#define DEBUG

constexpr auto MORPH_OPERATOR{ "MorphOperator" }; // MorphTypes
constexpr auto MORPH_SIZE{ "MorphSize" };
constexpr auto MORPH_ELEMENT{ "MorphElement" }; // MorphShapes

Filters::MorphologyOperation::MorphologyOperation(QJsonObject const &a_config)
	: m_morphOperator{ a_config[MORPH_OPERATOR].toInt() }
	, m_morphSize{ a_config[MORPH_SIZE].toInt() }
	, m_morphElement{ a_config[MORPH_ELEMENT].toInt() }
{
	#ifdef DEBUG
	//Logger->debug("Filters::MorphologyOperation::MorphologyOperation()");
	#endif
}

void Filters::MorphologyOperation::process(std::vector<_data> &_data)
{
	if (_data[0].processing.empty())
	{
		//Logger->error("Filters::MorphologyOperation::process() image is empty!");
	} 

	assert(_data[0].processing.empty() == false);
	cv::Mat m_Element = cv::getStructuringElement(m_morphElement, cv::Size(2 * m_morphSize + 1, 2 * m_morphSize + 1),
																								cv::Point(m_morphSize, m_morphSize));
	morphologyEx(_data[0].processing, _data[0].processing, m_morphOperator, m_Element);
}
