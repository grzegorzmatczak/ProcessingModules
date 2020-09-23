#include <QJsonObject>

#include "opencv_addweighted.h"

constexpr auto ALPHA{ "Alpha" };
constexpr auto BETA{ "Beta" };
constexpr auto GAMMA{ "Gamma" };

Adders::AddWeighted::AddWeighted(QJsonObject const &a_config)
  : m_alpha{ a_config[ALPHA].toDouble() }
  , m_beta{ a_config[BETA].toDouble() }
  , m_gamma{ a_config[GAMMA].toDouble() }
{
}

void Adders::AddWeighted::process(std::vector<_data> &_data)
{
   //H_logger->info("Adders::AddWeighted::process");
  cv::addWeighted(_data[0].processing, m_alpha, _data[1].processing, m_beta, m_gamma, _data[0].processing);
}
