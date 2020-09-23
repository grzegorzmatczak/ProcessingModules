#include <QJsonObject>

#include "opencv_add.h"

Adders::Add::Add(QJsonObject const &a_config)
{
}

void Adders::Add::process(std::vector<_data> &_data)
{
   //H_logger->info("Adders::Add::process");
  _data[0].processing = _data[0].processing + _data[1].processing;
}
