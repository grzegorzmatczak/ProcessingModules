#ifndef FILTER_ADD_MULTIPLE_DRON_BLURRED_H
#define FILTER_ADD_MULTIPLE_DRON_BLURRED_H

#include "basefilter.h"

struct boundsBlurred {
  qint32 x1;
  qint32 x2;
  qint32 y1;
  qint32 y2;
};

/* Paste in configuration:
[{
  "name": "AddDron_Average",
  "min": 1,
  "max": 23,
  "isOdd": true
},{
  "name": "AddDron_StandardDeviation",
  "min": 1,
  "max": 23,
  "isOdd": true
}],
*/

class QJsonObject;

namespace Filters {
class AddMultipleDronBlurred : public BaseFilter {
 public:
  AddMultipleDronBlurred(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

  void checkBoundies(const qint32 &offset, qint32 &x, qint32 &y, const struct boundsBlurred &b);

  void addGaussianNoise(cv::Mat &image, double average, double standard_deviation, cv::Mat &noise);

 private:
  int m_sizeMin{};
  int m_sizeMax{};
  int m_color{};
  int m_rotate{};
  int m_velocityMin{};
  int m_velocityMax{};
  double m_probabilityOfChangeSize{};
  double m_probabilityOfChangeVelocity{};
  double m_probabilityOfRotate{};
  int m_startGT{};
  int m_unknownGTColor{};
  int m_randSeed{};
  int m_noise_int{};
  double m_noise_double{};
  int m_oldRandX{};
  int m_oldRandY{};

 private:
  int oldRandX{};
  int oldRandY{};
  int m_randX{};
  int m_randY{};
  int dronVelocity{};
  int velocityX{};
  int velocityY{};
  int dronSize{};
  int dronSizeOld{};
  int offset{};
  int m_width{};
  int m_height{};
  bool m_firstTime{};
  int m_iterator{};
  bool m_globalOffset{};
  int m_markerType{};

 private:
  QRandomGenerator *m_randomGenerator;

 private:
  qint32 m_offset;
  qint32 m_dronThickness;
  qint32 m_clusterWidth;
  qint32 m_clusterHeight;
  std::vector<qint32> m_X;
  std::vector<qint32> m_Y;
  std::vector<qint32> m_oldX;
  std::vector<qint32> m_oldY;
  std::vector<qint32> m_velocityX;
  std::vector<qint32> m_velocityY;
  std::vector<qint32> m_markerTypeVec;
  std::vector<qint32> m_dronSize;
  std::vector<qint32> m_clusterOffset;

  std::vector<struct boundsBlurred> m_bounds;
};
} // namespace Filters

#endif // FILTER_ADD_MULTIPLE_DRON_BLURRED_H
