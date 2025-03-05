#include "opencv_bilateralfilter.h"

#include <QJsonObject>

// Diameter - diameter of each pixel neighborhood that is used during filtering.
// If it is non-positive, it is computed from sigmaSpace.
constexpr auto DIAMETER{ "Diameter" };
// SigmaColor - Filter sigma in the color space.
// A larger value of the parameter means that farther colors within the pixel neighborhood
// (see sigmaSpace) will be mixed together, resulting in larger areas of semi-equal color.
constexpr auto SIGMACOLOR{ "SigmaColor" };
// SigmaSpace - Filter sigma in the coordinate space.
// A larger value of the parameter means that farther pixels will influence each other
// as long as their colors are close enough (see sigmaColor ).
// When d>0, it specifies the neighborhood size regardless of sigmaSpace.
// Otherwise, d is proportional to sigmaSpace.
constexpr auto SIGMASPACE{ "SigmaSpace" };
// BorderType - border mode used to extrapolate pixels outside of the image
// enum BorderTypes:
// BORDER_CONSTANT    = 0,
// BORDER_REPLICATE   = 1,
// BORDER_REFLECT     = 2,
// BORDER_WRAP        = 3,
// BORDER_REFLECT_101 = 4,
// BORDER_TRANSPARENT = 5,
// BORDER_REFLECT101  = BORDER_REFLECT_101,
// BORDER_DEFAULT     = BORDER_REFLECT_101,
// BORDER_ISOLATED    = 16
constexpr auto BORDER_TYPE{ "BorderType" };

Filters::BilateralFilter::BilateralFilter(QJsonObject const &a_config)
  : m_diameter{ a_config[DIAMETER].toInt() }
  , m_sigmaColor{ a_config[SIGMACOLOR].toDouble() }
  , m_sigmaSpace{ a_config[SIGMASPACE].toDouble() }
  , m_borderType{ a_config[BORDER_TYPE].toInt() }
{
}

void Filters::BilateralFilter::process(std::vector<_data> &_data)
{
  cv::Mat dst = _data[0].processing.clone();
  cv::bilateralFilter(dst, _data[0].processing, m_diameter, m_sigmaColor, m_sigmaSpace, m_borderType);
}
