#ifndef COMPARES_VOT_CHANLLENGE_H
#define COMPARES_VOT_CHANLLENGE_H

#include "basecompare.h"

class QJsonObject;

namespace Compares {
class VOT : public BaseCompare {
public:
  VOT(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

private:
  regionBounds regionComputeBounds(const regionContainer &region);
  regionBounds regionCreateBounds(float left, float top, float right,
                                  float bottom);
  regionBounds boundsIntersection(regionBounds a, regionBounds b);
  regionBounds boundsRound(regionBounds bounds);
  regionOverlap regionComputeOverlap(const regionContainer &ra,
                                     const regionContainer &rb,
                                     regionBounds bounds);
  float boundsOverlap(regionBounds a, regionBounds b);
  regionContainer *regionCreateMask(int x, int y, int width, int height);
  regionContainer *createRegion(regionType type);

private:
};
} // namespace Compares

#endif // COMPARES_VOT_CHANLLENGE_H
