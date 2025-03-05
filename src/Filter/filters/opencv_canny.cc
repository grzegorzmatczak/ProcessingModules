#include "opencv_canny.h"

#include <QJsonObject>

constexpr auto TH1{"Threshold1"};
constexpr auto TH2{"Threshold2"};
constexpr auto APER_SIZE{"ApertureSize"};
constexpr auto L2_GRADIENT{"L2Gradient"};

Filters::Canny::Canny(QJsonObject const &a_config)
    : m_threshold1{a_config[TH1].toDouble()},
      m_threshold2{a_config[TH2].toDouble()},
      m_apertureSize{a_config[APER_SIZE].toInt()},
      m_L2Gradient{a_config[L2_GRADIENT].toBool()} {}

void Filters::Canny::process(std::vector<_data> &_data) {
  cv::Canny(_data[0].processing, _data[0].processing, m_threshold1,
            m_threshold2, m_apertureSize, m_L2Gradient);
}
