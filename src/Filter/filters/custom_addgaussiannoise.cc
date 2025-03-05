#include "custom_addgaussiannoise.h"

#include <QJsonObject>

#include <opencv2/opencv.hpp>

constexpr auto AVERAGE{ "Average" };
constexpr auto STANDARD_DEVIATION{ "StandardDeviation" };
constexpr auto RETURN_NOISE{ "ReturnNoise" };
#define DEBUG FALSE

Filters::AddGaussianNoise::AddGaussianNoise(QJsonObject const &a_config)
  : m_average{ a_config[AVERAGE].toDouble() }
  , m_standardDeviation{ a_config[STANDARD_DEVIATION].toInt() }
  , m_returnNoise{ a_config[RETURN_NOISE].toBool() }
{
}

void Filters::AddGaussianNoise::process(std::vector<_data> &_data)
{

 #if (DEBUG)
  cv::imshow("img", _data[0].processing);
 #endif
  cv::Mat noise = cv::Mat(_data[0].processing.rows, _data[0].processing.cols, CV_8UC1, cv::Scalar(255));
  cv::Mat noise_image(_data[0].processing.size(), CV_16SC1);
  double m_noise_double = m_standardDeviation / 1.0;
  cv::randn(noise_image, cv::Scalar::all(m_average), cv::Scalar::all(m_noise_double));

  if (m_returnNoise){
    noise_image.convertTo(_data[0].processing, _data[0].processing.type());
  } else {
    cv::Mat temp_image;
    _data[0].processing.convertTo(temp_image, CV_16SC1);
    addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
    temp_image.convertTo(_data[0].processing, _data[0].processing.type());
  }
#if (DEBUG)
  cv::imshow("noise", _data[0].processing);
  cv::waitKey(0);
#endif
}