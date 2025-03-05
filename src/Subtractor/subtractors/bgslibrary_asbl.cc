#include "bgslibrary_asbl.h"

#include <QJsonObject>
//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto ALPHA_LEARN{ "AlphaLearn" };
constexpr auto ALPHA_DETECTION{ "AlphaDetection" };
constexpr auto LEARNING_FRAMES{ "LearningFrames" };
constexpr auto THRESHOLD{ "Threshold" };


Subtractors::ASBL::ASBL(QJsonObject const &a_config) :
	alphaLearn{ a_config[ALPHA_LEARN].toDouble() },
	alphaDetection{ a_config[ALPHA_DETECTION].toDouble() },
	learningFrames{ a_config[LEARNING_FRAMES].toInt() },
	threshold{ a_config[THRESHOLD].toInt() },
	counter(0), 
	minVal(0.0),
	maxVal(1.0)
{
	//debug_construction(AdaptiveBackgroundLearning);
	//initLoadSaveConfig(algorithmName);
	#ifdef DEBUG
	Logger->debug("Subtractors::ASBL::ASBL()");
	#endif
}

Subtractors::ASBL::~ASBL()
{
	//debug_destruction(AdaptiveBackgroundLearning);
	#ifdef DEBUG
	Logger->debug("Subtractors::ASBL::~ASBL()");
	#endif
}

//void ASBL::process(const cv::Mat &img_input_, cv::Mat &img_output, cv::Mat &img_bgmodel)
void Subtractors::ASBL::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	Logger->debug("Subtractors::ASBL::process()");
	#endif
	assert(_data[0].processing.empty() == false);
	cv::Mat img_input;
	if (_data[0].processing.channels() == 3)
	{
		cv::cvtColor(_data[0].processing, img_input, cv::COLOR_BGR2GRAY);
	}
	else
	{
		_data[0].processing.copyTo(img_input);
	}

	if (img_background.empty())
	{
		img_input.copyTo(img_background);
	}

	cv::Mat img_input_f(img_input.size(), CV_32F);
	img_input.convertTo(img_input_f, CV_32F, 1. / 255.);

	cv::Mat img_background_f(img_background.size(), CV_32F);
	img_background.convertTo(img_background_f, CV_32F, 1. / 255.);

	cv::Mat img_diff_f(img_input.size(), CV_32F);
	cv::absdiff(img_input_f, img_background_f, img_diff_f);

	//cv::Mat img_foreground(img_input.size(), CV_8U);
	img_diff_f.convertTo(img_foreground, CV_8U, 255.0 / (maxVal - minVal), -minVal);

	cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
	cv::medianBlur(img_foreground, img_foreground, 3);

	if (learningFrames > 0 && counter <= learningFrames)
	{
		//std::cout << "Adaptive update..." << std::endl;
		// Only Adaptive update of the background model
		img_background_f = alphaLearn * img_input_f + (1 - alphaLearn) * img_background_f;
		counter++;
	}
	else
	{
		//std::cout << "Adaptive and Selective update..." << std::endl;
		int rows = img_input.rows;
		int cols = img_input.cols;
		for (int i = 0; i < rows; i++) 
		{
			for (int j = 0; j < cols; j++) 
			{
				// Adaptive and Selective update of the background model
				if (img_foreground.at<uchar>(i, j) == 0) 
				{
					img_background_f.at<float>(i, j) = alphaDetection * img_input_f.at<float>(i, j) + (1 - alphaDetection) * img_background_f.at<float>(i, j);
				}
			}
		}
	}

	//cv::Mat img_new_background(img_input.size(), CV_8U);
	img_background_f.convertTo(img_background, CV_8UC1, 255.0 / (maxVal - minVal), -minVal);
	//img_new_background.copyTo(img_background);

	#ifdef DEBUG_OPENCV
		cv::imshow("ABL_FG", img_foreground);
		cv::imshow("ABL_BG", img_background);
		cv::waitKey(0);
	#endif

	//img_foreground.copyTo(img_output);
	img_foreground.copyTo(_data[0].processing);
	//img_background.copyTo(img_bgmodel);
}