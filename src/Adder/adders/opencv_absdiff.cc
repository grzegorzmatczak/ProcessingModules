#include <QJsonObject>

#include "opencv_absdiff.h"

Adders::AbsDiff::AbsDiff() {}

void Adders::AbsDiff::process(std::vector<_data> &_data)
{
    if (_data[0].processing.empty())
    {
        //spdlog::error("Adders::AbsDiff::process() image 1 is empty!");
    }
    else
    {
        //Logger->trace("Adders::AbsDiff::process() image 1 is correct");
    }
    assert(_data[0].processing.empty() == false);

    if (_data[1].processing.empty())
    {
        //spdlog::error("Adders::AbsDiff::process() image 2 is empty!");
    }
    else
    {
        //Logger->trace("Adders::AbsDiff::process() image 2 is correct");
    }
    assert(_data[1].processing.empty() == false);
    cv::absdiff(_data[0].processing, _data[1].processing, _data[0].processing);
}
