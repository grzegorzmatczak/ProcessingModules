#ifndef BASECOMPARE_H
#define BASECOMPARE_H

#include <opencv2/imgproc.hpp>
#include "../compare.h"

class BaseCompare {
public:
  BaseCompare();
  virtual ~BaseCompare();

  virtual void process(std::vector<_data> &_data) = 0;
};

#endif // BASECOMPARE_H
