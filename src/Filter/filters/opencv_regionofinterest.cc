#include "opencv_regionofinterest.h"
#include <QJsonObject>

Filters::RegionOfInterest::RegionOfInterest(QJsonObject const &a_config)
: m_width{a_config[WIDTH].toInt()}, m_height{a_config[HEIGHT].toInt()} 
, m_x{a_config[X].toInt()}, m_y{a_config[Y].toInt()}
{
} 

void Filters::RegionOfInterest::process(std::vector<_data> &_data)
{
  //cv::resize(_data[0].processing, _data[0].processing, cv::Size(m_width, m_height));
  cv::Rect region_of_interest = cv::Rect(m_x, m_y, m_width, m_height);
  _data[0].processing = _data[0].processing(region_of_interest);
}
