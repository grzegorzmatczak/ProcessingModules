#include "opencv_inrange.h"
#include <QJsonObject>

constexpr auto LOWERB1{"Lowerb1"};
constexpr auto LOWERB2{"Lowerb2"};
constexpr auto LOWERB3{"Lowerb3"};
constexpr auto UPPERB1{"Upperb1"};
constexpr auto UPPERB2{"Upperb2"};
constexpr auto UPPERB3{"Upperb3"};

Filters::InRange::InRange(QJsonObject const &a_config)
: 	m_lowerb1{a_config[LOWERB1].toInt()},
	m_lowerb2{a_config[LOWERB2].toInt()},
	m_lowerb3{a_config[LOWERB3].toInt()},
	m_upperb1{a_config[UPPERB1].toInt()},
	m_upperb2{a_config[UPPERB2].toInt()}, 
	m_upperb3{a_config[UPPERB3].toInt()}
{
	if (m_lowerb1 > m_upperb1)
	{
		int temp = m_lowerb1;
		m_lowerb1 = m_upperb1;
		m_upperb1 = temp;
	}
	else if (m_lowerb1 == m_upperb1)
	{
		if(m_lowerb1 != 0)
		{
			m_lowerb1--;
		}
		else if(m_upperb1 != 255)
		{
			m_upperb1++;
		}
	}
}

void Filters::InRange::process(std::vector<_data> &_data)
{
	if (_data[0].processing.channels() == 3)
	{
		cv::inRange(_data[0].processing, cv::Scalar(m_lowerb1, m_lowerb2, m_lowerb3),
				cv::Scalar(m_upperb1, m_upperb2, m_upperb3), _data[0].processing);
	}
	else if (_data[0].processing.channels() == 1)
	{
		cv::inRange(_data[0].processing, cv::Scalar(m_lowerb1), cv::Scalar(m_upperb1), _data[0].processing);
	}
}
