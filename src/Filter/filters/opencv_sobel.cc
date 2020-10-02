#include "opencv_sobel.h"
#include <QJsonObject>

constexpr auto ORDERX{ "OrderDerivativeX" };
constexpr auto ORDERY{ "OrderDerivativeY" };
constexpr auto KERNELSIZE{ "KernelSize" };
constexpr auto BORDER_TYPE{ "BorderType" };

Filters::Sobel::Sobel(QJsonObject const &a_config)
  : m_orderDerivativeX{ a_config[ORDERX].toInt() }
  , m_orderDerivativeY{ a_config[ORDERY].toInt() }
  , m_kernelSize{ a_config[KERNELSIZE].toInt() }
  , m_scale(1)
  , m_delta(0)
  , m_borderType{ a_config[BORDER_TYPE].toInt() }
{
}

void Filters::Sobel::process(std::vector<_data> &_data)
{

  cv::Mat dst = _data[0].processing.clone();
  cv::Mat grad_x, grad_y;
  cv::Mat abs_grad_x, abs_grad_y;
  cv::Sobel(dst, grad_x, -1, m_orderDerivativeX, 0, m_kernelSize, m_scale, m_delta, m_borderType);
  cv::Sobel(dst, grad_y, -1, 0, m_orderDerivativeY, m_kernelSize, m_scale, m_delta, m_borderType);

  cv::convertScaleAbs(grad_x, abs_grad_x);
  cv::convertScaleAbs(grad_y, abs_grad_y);

  cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, _data[0].processing);
}
