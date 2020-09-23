#ifndef SOBEL_H
#define SOBEL_H

#include "basefilter.h"

class QJsonObject;

/* Paste in configuration:
[{
  "name": "Sobel_OrderDerivativeX",
  "min": 1,
  "max": 2
},{
  "name": "Sobel_OrderDerivativeY",
  "min": 1,
  "max": 2
},{
  "name": "Sobel_KernelSize",
  "min": 1,
  "max": 7,
  "isOdd": true
},{
  "name": "Sobel_BorderType",
  "min": 1,
  "max": 2
}],
,{
        "Name": "C",
        "Active": 2,
        "Type": "Filter",
        "Next":[{"Name": "", "Active": -1}],
        "Prev":[{"Name": "B", "Active": 1}],
        "Config":{"Name": "Sobel", "OrderDerivativeX": 1,"OrderDerivativeY": 1 ,
"KernelSize": 3, "BorderType": 1}
    }

*/

namespace Filters {
class Sobel : public BaseFilter {
public:
  Sobel(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  int const m_orderDerivativeX;
  int const m_orderDerivativeY;
  int const m_kernelSize;
  int const m_borderType;
  double m_scale;
  double m_delta;
};
} // namespace Filters

#endif // SOBEL_H
