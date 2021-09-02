#ifndef SUBTRACTORS_CUSTOM_VIBE_H
#define SUBTRACTORS_CUSTOM_VIBE_H

#include "basesubtractor.h"
#include "ViBe/vibe-background-sequential.h"

class QJsonObject;

namespace Subtractors 
{
class ViBe : public BaseSubtractor 
{
public:
  ViBe(QJsonObject const &a_config);
  ~ViBe();
  void process(std::vector<_data> &_data);

private:

  int m_numberOfSamples{};
  int m_matchingThreshold{};
  int m_matchingNumber{};
  int m_updateFactor{};
  vibe::vibeModel_Sequential_t *model;
  bool m_firstTime{};
};
} // namespace Subtractors

#endif // SUBTRACTORS_CUSTOM_VIBE_H
