#include "bgslibrary_abl.h"

#include <QJsonObject>

//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto ALPHA{ "Alpha" };


Subtractors::ABL::ABL(QJsonObject const &a_config) :
	alpha{ a_config[ALPHA].toDouble() },
	limit(-1), 
	counter(0), 
	minVal(0.0),
	maxVal(1.0), 
	enableThreshold(false), 
	threshold(15)
{
	//debug_construction(AdaptiveBackgroundLearning);
	//initLoadSaveConfig(algorithmName);
	#ifdef DEBUG
	Logger->debug("Subtractors::ABL::ABL()");
	#endif
}

Subtractors::ABL::~ABL()
{
	//debug_destruction(AdaptiveBackgroundLearning);
	#ifdef DEBUG
	Logger->debug("Subtractors::ABL::~ABL()");
	#endif
}

//void ABL::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
void Subtractors::ABL::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::ABL::process()");
	#endif
	assert(_data[0].processing.empty() == false);
	if (img_background.empty())
	{
		_data[0].processing.copyTo(img_background);
	}

	cv::Mat img_output = cv::Mat::zeros(_data[0].processing.size(), CV_8UC1);
	cv::Mat img_bgmodel = cv::Mat::zeros(_data[0].processing.size(), CV_8UC3);

	cv::Mat img_input_f(_data[0].processing.size(), CV_32F);
	_data[0].processing.convertTo(img_input_f, CV_32F, 1. / 255.);

	cv::Mat img_background_f(img_background.size(), CV_32F);
	img_background.convertTo(img_background_f, CV_32F, 1. / 255.);

	cv::Mat img_diff_f(_data[0].processing.size(), CV_32F);
	cv::absdiff(img_input_f, img_background_f, img_diff_f);

	if ((limit > 0 && limit < counter) || limit == -1)
	{
		img_background_f = alpha*img_input_f + (1 - alpha)*img_background_f;

		cv::Mat img_new_background(_data[0].processing.size(), CV_8U);
		img_background_f.convertTo(img_new_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
		img_new_background.copyTo(img_background);

		if (limit > 0 && limit < counter)
		{
			counter++;
		}
	}

	//cv::Mat img_foreground(_data[0].processing.size(), CV_8U);
	img_diff_f.convertTo(img_foreground, CV_8UC1, 255.0 / (maxVal - minVal), -minVal);

	if (img_foreground.channels() == 3)
	{
		cv::cvtColor(img_foreground, img_foreground, cv::COLOR_BGR2GRAY);
	}

	if (enableThreshold)
	{
		cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
	}

	#ifdef DEBUG_OPENCV
		cv::imshow("ABL_FG", img_foreground);
		cv::imshow("ABL_BG", img_background);
		cv::waitKey(0);
	#endif

	//img_foreground.copyTo(img_output);
	img_foreground.copyTo(_data[0].processing);
	//_data[0].processing
	//img_background.copyTo(img_bgmodel);
	//firstTime = false;
	#ifdef DEBUG
	Logger->debug("Subtractors::ABL::process() done");
	#endif
}
