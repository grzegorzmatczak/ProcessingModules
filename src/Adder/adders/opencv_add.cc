#include <QJsonObject>

#include "opencv_add.h"

Adders::Add::Add(QJsonObject const &a_config)
{
  //Logger->debug("Adders::Add::Add()");
}

void Adders::Add::process(std::vector<_data> &_data)
{
	if (_data[0].processing.empty())
    {
    //spdlog::error("Adders::Add::process() image 1 is empty!");
  } else {
   // Logger->trace("Adders::Add::process() image 1 is correct");
  }
  assert(_data[0].processing.empty() == false);

  if (_data[1].processing.empty()) {
    //spdlog::error("Adders::Add::process() image 2 is empty!");
  } else {
    //Logger->trace("Adders::Add::process() image 2 is correct");
  }
  assert(_data[1].processing.empty() == false);

  _data[0].processing = _data[0].processing + _data[1].processing;
}
