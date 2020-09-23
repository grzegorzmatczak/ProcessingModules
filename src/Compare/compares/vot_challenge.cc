#include "vot_challenge.h"

Compares::VOT::VOT(QJsonObject const &a_config) {}

void Compares::VOT::process(std::vector<_data> &_data) {
  cv::Mat dsst = _data[0].processing.clone();
}

regionBounds Compares::VOT::regionComputeBounds(const regionContainer &region) {
  regionBounds bounds;
  switch (region.type) {
  case RECTANGLE:
    bounds = VOT::regionCreateBounds(
        region.data.rectangle.x, region.data.rectangle.y,
        region.data.rectangle.x + region.data.rectangle.width,
        region.data.rectangle.y + region.data.rectangle.height);
    break;
  default: {
    bounds = regionBounds{0, 0, 0, 0};
  }
  }
  return bounds;
}

regionBounds Compares::VOT::regionCreateBounds(float left, float top,
                                               float right, float bottom) {
  regionBounds bounds;
  bounds.top = top;
  bounds.bottom = bottom;
  bounds.left = left;
  bounds.right = right;
  return bounds;
}

regionBounds Compares::VOT::boundsIntersection(regionBounds a, regionBounds b) {
  regionBounds bounds;
  bounds.top = MAX(a.top, b.top);
  bounds.bottom = MIN(a.bottom, b.bottom);
  bounds.left = MAX(a.left, b.left);
  bounds.right = MIN(a.right, b.right);
  return bounds;
}

regionBounds Compares::VOT::boundsRound(regionBounds bounds) {

  bounds.top = floor(bounds.top);
  bounds.bottom = floor(bounds.bottom);
  bounds.left = floor(bounds.left);
  bounds.right = floor(bounds.right);
  return bounds;
}

regionOverlap Compares::VOT::regionComputeOverlap(const regionContainer &ra,
                                                  const regionContainer &rb,
                                                  regionBounds bounds) {
  int x, y;
  int width, height;
  regionBounds b1, b2;
  double a1, a2;
  int mask_intersect = 0;

  regionOverlap ovelap{0, 0, 0};

  b1 = boundsIntersection(boundsRound(regionComputeBounds(ra)), bounds);
  b2 = boundsIntersection(boundsRound(regionComputeBounds(rb)), bounds);

  x = MIN(b1.left, b2.left);
  y = MIN(b1.top, b2.top);
  width = (MAX(b1.right, b2.right) - x) + 1;
  height = (MAX(b1.bottom, b2.bottom) - y) + 1;

  // Check if the ratio between the two b.boxes is simply too big
  a1 = (b1.right - b1.left) * (b1.bottom - b1.top);
  a2 = (b2.right - b2.left) * (b2.bottom - b2.top);
  if (a1 / a2 < 1e-10 || a2 / a1 < 1e-10 || width < 1 || height < 1) {
    return ovelap;
  }
  if (boundsOverlap(b1, b2) == 0) {
    int i;
    // ta = region_

  } else {
    int i;
    char *mask1;
    char *mask2;

    regionContainer *ta = NULL;
    regionContainer *tb = NULL;
    ta = regionCreateMask(x, y, width, height);
    tb = regionCreateMask(x, y, width, height);

    mask1 = ta->data.mask.data;
    mask2 = tb->data.mask.data;
  }
}

float Compares::VOT::boundsOverlap(regionBounds a, regionBounds b) {

  regionBounds rIntersection = boundsIntersection(a, b);
  float intersection = (rIntersection.right - rIntersection.left) *
                       (rIntersection.bottom - rIntersection.top);
  return MAX(0, intersection / (((a.right - a.left) * (a.bottom - a.top)) *
                                    ((b.right - b.left) * (b.bottom - b.top)) -
                                intersection));
}

regionContainer *Compares::VOT::regionCreateMask(int x, int y, int width,
                                                 int height) {

  assert(width > 0 && height > 0);
  {
    regionContainer *reg = createRegion(MASK);
    reg->data.mask.x = x;
    reg->data.mask.y = y;
    reg->data.mask.width = width;
    reg->data.mask.height = height;
    reg->data.mask.data = new char(sizeof(char) * width * height);
    return reg;
  }
}

regionContainer *Compares::VOT::createRegion(regionType type) {

  regionContainer *reg = new regionContainer();
  reg->type = type;
  return reg;
}
