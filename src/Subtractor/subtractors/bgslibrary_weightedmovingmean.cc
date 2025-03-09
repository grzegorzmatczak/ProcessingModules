#include "bgslibrary_weightedmovingmean.h"

#include <QJsonObject>
//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto ENABLE_WEIGHT{ "EnableWeight" };
constexpr auto ENABLE_THRESHOLD{ "EnableThreshold" };
constexpr auto THRESHOLD{ "Threshold" };


Subtractors::WeightedMovingMean::WeightedMovingMean(QJsonObject const &a_config) :
	enableWeight{ a_config[ENABLE_WEIGHT].toBool() },
	enableThreshold{ a_config[ENABLE_THRESHOLD].toBool() },
	threshold{ a_config[THRESHOLD].toInt() }
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingMean::WeightedMovingMean()");
	#endif
}

Subtractors::WeightedMovingMean::~WeightedMovingMean()
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingMean::~WeightedMovingMean()");
	#endif
}

void Subtractors::WeightedMovingMean::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingMean::process()");
	#endif
	assert(_data[0].processing.empty() == false);

	if (img_input_prev_1.empty())
	{
		_data[0].processing.copyTo(img_input_prev_1);
		return;
	}

	if (img_input_prev_2.empty())
	{
		img_input_prev_1.copyTo(img_input_prev_2);
		_data[0].processing.copyTo(img_input_prev_1);
		return;
	}

	cv::Mat img_input_f(_data[0].processing.size(), CV_32F);
	_data[0].processing.convertTo(img_input_f, CV_32F, 1. / 255.);

	cv::Mat img_input_prev_1_f(_data[0].processing.size(), CV_32F);
	img_input_prev_1.convertTo(img_input_prev_1_f, CV_32F, 1. / 255.);

	cv::Mat img_input_prev_2_f(_data[0].processing.size(), CV_32F);
	img_input_prev_2.convertTo(img_input_prev_2_f, CV_32F, 1. / 255.);

	cv::Mat img_background_f(_data[0].processing.size(), CV_32F);

	if (enableWeight)
	{
		img_background_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
	}
	else
	{
		img_background_f = ((img_input_f)+(img_input_prev_1_f)+(img_input_prev_2_f)) / 3.0;
	}

	double minVal, maxVal;
	minVal = 0.; maxVal = 1.;
	img_background_f.convertTo(img_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);

	cv::absdiff(_data[0].processing, img_background, img_foreground);

	if (img_foreground.channels() == 3)
	{
		cv::cvtColor(img_foreground, img_foreground, cv::COLOR_BGR2GRAY);
	}

	if (enableThreshold)
	{
		cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
	}

	#ifdef DEBUG_OPENCV
		cv::imshow("_data[0].processing_FG", img_foreground);
		cv::imshow("img_foreground_FG", img_foreground);
	#endif

	img_foreground.copyTo(_data[0].processing);
	m_firstTime = false;

	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingMean::process() done");
	#endif
}