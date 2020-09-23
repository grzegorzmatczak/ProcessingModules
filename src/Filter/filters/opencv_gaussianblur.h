#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

#include "basefilter.h"

class QJsonObject;

/* Paste in configuration:
[{
  "name": "GaussianBlur_KernelSizeX",
  "min": 1,
  "max": 23,
  "isOdd": true
},{
  "name": "GaussianBlur_KernelSizeY",
  "min": 1,
  "max": 23,
  "isOdd": true
},{
  "name": "GaussianBlur_m_sigmaX",
  "min": 0,
  "max": 1,
  "isDouble": 100
},{
  "name": "GaussianBlur_m_sigmaY",
  "min": 0,
  "max": 1,
  "isDouble": 100
},{
  "name": "GaussianBlur_BorderType",
  "min": 1,
  "max": 2
}],
*/

namespace Filters
{
class GaussianBlur : public BaseFilter
{
 public:
  GaussianBlur(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  int const m_kernelSizeX;
  int const m_kernelSizeY;
  double const m_sigmaX;
  double const m_sigmaY;
  int const m_borderType;
};
} // namespace Filters

#endif // GAUSSIANBLUR_H
