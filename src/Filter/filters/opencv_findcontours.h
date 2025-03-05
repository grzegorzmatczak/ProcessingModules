#ifndef FILTERS_OPENCV_FIND_CONTOURS_H
#define FILTERS_OPENCV_FIND_CONTOURS_H

#include "basefilter.h"



/* Paste in configuration:
*/

namespace Filters
{
class FindContours : public BaseFilter
{
 public:
  FindContours(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  bool const m_drawContoursValid;
  bool const m_drawContoursNotValid;
  double const m_minContourSize;
  double const m_maxContourSize;
  bool const m_saveContours;

  cv::Scalar m_colorValid;
  cv::Scalar m_colorNotValid;

};
} // namespace Filters

#endif // FILTERS_OPENCV_FIND_CONTOURS_H
