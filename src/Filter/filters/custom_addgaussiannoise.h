#ifndef FILTER_ADD_GAUSSIAN_NOISE_H
#define FILTER_ADD_GAUSSIAN_NOISE_H

#include "basefilter.h"

/* Paste in configuration:
[{
  "name": "AddGaussianNoise_Average",
  "min": 1,
  "max": 23,
  "isOdd": true
},{
  "name": "AddGaussianNoise_StandardDeviation",
  "min": 1,
  "max": 23,
  "isOdd": true
}],
*/

class QJsonObject;

namespace Filters
{
class AddGaussianNoise : public BaseFilter {
 public:
  AddGaussianNoise(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

 private:
  double const m_average;
  int const m_standardDeviation;
  bool const m_returnNoise;
};
} // namespace Filters

#endif // FILTER_ADD_GAUSSIAN_NOISE_H
