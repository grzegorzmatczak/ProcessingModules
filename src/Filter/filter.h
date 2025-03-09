#ifndef FILTER_H
#define FILTER_H

#include <QDebug>
#include <QObject>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "processing.h"

class BaseFilter;
class QJsonObject;

class Filter : public Processing
{
	Q_OBJECT
public:
	explicit Filter(QObject *parent = nullptr);
	~Filter();
	Filter(const Filter& other); // II. copy constructor
	Filter& operator=(const Filter& other); // III. copy assignment
	Filter(Filter&& other) noexcept; // IV. move constructor
	Filter& operator=(Filter&& other) noexcept; // V. move assignment
	void process(std::vector<_data> &_data);
	double getElapsedTime();
	void configure(QJsonObject const& a_config);
private:
	BaseFilter *m_baseFilter{};
	cv::TickMeter m_timer;
};

#endif // FILTER_H
