#ifndef FILTER_ADD_DRON_H
#define FILTER_ADD_DRON_H

#include "basefilter.h"

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
class AddDron : public BaseFilter {
 public:
  AddDron(QJsonObject const &a_config);

  void process(std::vector<_data> &_data);

  void checkBoundies(qint32 offset);

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
  int m_markerType{};
  int m_startGT{};
  int m_unknownGTColor{};
  int m_randSeed{};
  int m_noise_int{};
  double m_noise_double{};
  int m_oldRandX{};
  int m_oldRandY{};
  int m_velocityX{};
  int m_velocityY{};

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

  private:
  QRandomGenerator *m_randomGenerator;
};
} // namespace Filters

#endif // FILTER_ADD_DRON_H
