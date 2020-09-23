#ifndef BASE_ADDER_H
#define BASE_ADDER_H

#include <QDateTime>

#include "opencv2/bgsegm.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "../adder.h"

class BaseAdder {
public:
  BaseAdder();
  virtual ~BaseAdder();

  virtual void process(std::vector<_data> &_data) = 0;
};

#endif // BASE_SUBTRACTOR_H
