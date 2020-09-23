#include "opencv_medianblur.h"
#include <QJsonObject>

constexpr auto KERNEL_SIZE{ "KernelSize" };

Filters::MedianBlur::MedianBlur(QJsonObject const &a_config)
  : m_kernelSize{ a_config[KERNEL_SIZE].toInt() }
{
}

void Filters::MedianBlur::process(std::vector<_data> &_data)
{
  cv::medianBlur(_data[0].processing, _data[0].processing, m_kernelSize);
}
