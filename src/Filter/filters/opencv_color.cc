#include "opencv_color.h"

#include <QJsonObject>

constexpr auto COLOR_CODE{ "ColorCode" };

Filters::Color::Color(QJsonObject const &a_config)
  : m_colorCode{ a_config[COLOR_CODE].toInt() }
{
}

void Filters::Color::process(std::vector<_data> &_data)
{
  cv::cvtColor(_data[0].processing, _data[0].processing, m_colorCode);
}
