#ifndef BASEFILTER_H
#define BASEFILTER_H

#include "../filter.h"
#include "processing_global.h"

class BaseFilter {
public:
  BaseFilter();
  virtual ~BaseFilter();

  virtual void process(std::vector<_data> &_data) = 0;
};

#endif // BASEFILTER_H
