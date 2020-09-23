#ifndef BASE_SUBTRACTOR_H
#define BASE_SUBTRACTOR_H

#include <QDateTime>

#include "opencv2/bgsegm.hpp"
#include "opencv2/core.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include "../subtractor.h"

class BaseSubtractor {
public:
  BaseSubtractor();
  virtual ~BaseSubtractor();

  virtual void process(std::vector<_data> &_data) = 0;

  protected:
  bool m_firstTime = true;
};

#endif // BASE_SUBTRACTOR_H
