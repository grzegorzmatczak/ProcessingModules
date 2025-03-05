#include "bgslibrary_weightedmovingvariance.h"

#include <QJsonObject>

//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto ENABLE_WEIGHT{ "EnableWeight" };
constexpr auto ENABLE_THRESHOLD{ "EnableThreshold" };
constexpr auto THRESHOLD{ "Threshold" };


Subtractors::WeightedMovingVariance::WeightedMovingVariance(QJsonObject const &a_config) :
	enableWeight{ a_config[ENABLE_WEIGHT].toBool() },
	enableThreshold{ a_config[ENABLE_THRESHOLD].toBool() },
	threshold{ a_config[THRESHOLD].toInt() }
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingVariance::WeightedMovingVariance()");
	#endif
}

Subtractors::WeightedMovingVariance::~WeightedMovingVariance()
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingVariance::~WeightedMovingVariance()");
	#endif
}

void Subtractors::WeightedMovingVariance::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingVariance::process()");
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

	// Weighted mean
	cv::Mat img_mean_f(_data[0].processing.size(), CV_32F);

	if (enableWeight)
	{
		img_mean_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
	}
	else
	{
		img_mean_f = ((img_input_f * 0.3) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.3));
	}

	// Weighted variance
	cv::Mat img_1_f(_data[0].processing.size(), CV_32F);
	cv::Mat img_2_f(_data[0].processing.size(), CV_32F);
	cv::Mat img_3_f(_data[0].processing.size(), CV_32F);
	cv::Mat img_4_f(_data[0].processing.size(), CV_32F);

	if (enableWeight)
	{
		img_1_f = computeWeightedVariance(img_input_f, img_mean_f, 0.5);
		img_2_f = computeWeightedVariance(img_input_prev_1_f, img_mean_f, 0.3);
		img_3_f = computeWeightedVariance(img_input_prev_2_f, img_mean_f, 0.2);
		img_4_f = (img_1_f + img_2_f + img_3_f);
	}
	else
	{
		img_1_f = computeWeightedVariance(img_input_f, img_mean_f, 0.3);
		img_2_f = computeWeightedVariance(img_input_prev_1_f, img_mean_f, 0.3);
		img_3_f = computeWeightedVariance(img_input_prev_2_f, img_mean_f, 0.3);
		img_4_f = (img_1_f + img_2_f + img_3_f);
	}

	// Standard deviation
	cv::Mat img_sqrt_f(_data[0].processing.size(), CV_32F);
	cv::sqrt(img_4_f, img_sqrt_f);
	cv::Mat img_sqrt(_data[0].processing.size(), CV_8U);
	double minVal, maxVal;
	minVal = 0.; maxVal = 1.;
	img_sqrt_f.convertTo(img_sqrt, CV_8U, 255.0 / (maxVal - minVal), -minVal);
	img_sqrt.copyTo(img_foreground);

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

	img_input_prev_1.copyTo(img_input_prev_2);
	_data[0].processing.copyTo(img_input_prev_1);

	m_firstTime = false;

	#ifdef DEBUG
	Logger->debug("Subtractors::WeightedMovingVariance::process() done");
	#endif
}

cv::Mat Subtractors::WeightedMovingVariance::computeWeightedVariance(const cv::Mat &img_input_f, const cv::Mat &img_mean_f, 
const double weight)
{
  //ERROR in return (weight * ((cv::abs(img_input_f - img_mean_f))^2.));
  cv::Mat img_f_absdiff(img_input_f.size(), CV_32F);
  cv::absdiff(img_input_f, img_mean_f, img_f_absdiff);
  cv::Mat img_f_pow(img_input_f.size(), CV_32F);
  cv::pow(img_f_absdiff, 2.0, img_f_pow);
  cv::Mat img_f = weight * img_f_pow;
  return img_f;
}