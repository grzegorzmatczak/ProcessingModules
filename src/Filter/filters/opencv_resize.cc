#include "opencv_resize.h"
#include <QJsonObject>

Filters::Resize::Resize(QJsonObject const &a_config)
    : m_width{a_config[WIDTH].toInt()}, m_height{a_config[HEIGHT].toInt()} {}

void Filters::Resize::process(std::vector<_data> &_data) {
  cv::resize(_data[0].processing, _data[0].processing,
             cv::Size(m_width, m_height));
}
