#include "opencv_blur.h"

#include <QJsonObject>

constexpr auto KERNEL_SIZE_X{ "KernelSizeX" };
constexpr auto KERNEL_SIZE_Y{ "KernelSizeY" };
constexpr auto BORDER_TYPE{ "BorderType" };

Filters::Blur::Blur(QJsonObject const &a_config)
  : m_kernelSizeX{ a_config[KERNEL_SIZE_X].toInt() }
  , m_kernelSizeY{ a_config[KERNEL_SIZE_Y].toInt() }
  , m_borderType{ a_config[BORDER_TYPE].toInt() }
{
}

void Filters::Blur::process(std::vector<_data> &_data)
{
  cv::blur(_data[0].processing, _data[0].processing, cv::Size(m_kernelSizeX, m_kernelSizeY), cv::Point(-1, -1),
           m_borderType);
}
