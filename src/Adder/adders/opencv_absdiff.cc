#include <QJsonObject>

#include "opencv_absdiff.h"


Adders::AbsDiff::AbsDiff() {
  qInfo() << "Creating AbsDiff: ";
}

void Adders::AbsDiff::process(std::vector<_data> &_data)
{
  qInfo() << "AbsDiff::process: " ;
  qInfo() << "AbsDiff::process: _data.size" << _data.size();
   //H_logger->info("Adders::AddWeighted::process");
  cv::absdiff(_data[0].processing, _data[1].processing, _data[0].processing);
  // _data[0].processing = cv::absdiff(_data[0].processing, _data[1].processing);

}
