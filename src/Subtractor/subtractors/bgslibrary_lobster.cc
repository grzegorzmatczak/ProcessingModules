#include "bgslibrary_lobster.h"

#include <QJsonObject>

//#define DEBUG
//#define DEBUG_OPENCV

constexpr auto REL_SIMILARITY_THRESHOLD{ "RelSimilarityThreshold" };
constexpr auto OFFSET_SIMILARITY_THRESHOLD{ "OffsetSimilarityThreshold" };
constexpr auto DESC_DIST_THRESHOLD{ "DescDistThreshold" };
constexpr auto COLOR_DIST_THRESHOLD{ "ColorDistThreshold" };
constexpr auto NB_BG_SAMPLES{ "NbBGSamples" };
constexpr auto REQUIRED_NB_BG_SAMPLES{ "RequiredNbBgSamples" };
constexpr auto LEARNING_RATE{ "LearningRate" };


Subtractors::LOBSTER::LOBSTER(QJsonObject const &a_config) :
m_relThreshold{ a_config[REL_SIMILARITY_THRESHOLD].toDouble() },
m_offsetSimilarityThreshold{  size_t(a_config[OFFSET_SIMILARITY_THRESHOLD].toInt()) },
m_descDistThreshold{ size_t(a_config[DESC_DIST_THRESHOLD].toInt()) },
m_colorDistThreshold{  size_t(a_config[COLOR_DIST_THRESHOLD].toInt()) },
m_nbBGSamples{  size_t(a_config[NB_BG_SAMPLES].toInt()) },
m_requiredNbBGSamples{  size_t(a_config[REQUIRED_NB_BG_SAMPLES].toInt()) },
m_learnignRate{  size_t(a_config[LEARNING_RATE].toInt()) },
m_firstTime(true)
{
	//debug_construction(AdaptiveBackgroundLearning);
	//initLoadSaveConfig(algorithmName);
	#ifdef DEBUG
	Logger->debug("Subtractors::LOBSTER::LOBSTER()");
	#endif

	pLOBSTER = new lbsp::BackgroundSubtractorLOBSTER(
		m_relThreshold, m_offsetSimilarityThreshold, m_descDistThreshold,
		m_colorDistThreshold, m_nbBGSamples, m_requiredNbBGSamples);
	
}



Subtractors::LOBSTER::~LOBSTER()
{
	//debug_destruction(AdaptiveBackgroundLearning);
	#ifdef DEBUG
	Logger->debug("Subtractors::LOBSTER::~LOBSTER()");
	#endif
}

void Subtractors::LOBSTER::process(std::vector<_data> &_data)
{
	if(m_firstTime)
	{
		#ifdef DEBUG
		Logger->debug("Subtractors::LOBSTER::process() m_firstTime");
		#endif
		pLOBSTER->initialize(_data[0].processing, cv::Mat(_data[0].processing.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
		m_firstTime = false;
	}

	#ifdef DEBUG
	Logger->debug("Subtractors::LOBSTER::process() apply");
	#endif
	pLOBSTER->apply(_data[0].processing, img_foreground);
	#ifdef DEBUG
	Logger->debug("Subtractors::LOBSTER::process() getBackgroundImage");
	#endif
	pLOBSTER->getBackgroundImage(img_background);

	#ifdef DEBUG_OPENCV
		cv::imshow("ABL_FG", img_foreground);
		cv::imshow("ABL_BG", img_background);
		cv::waitKey(0);
	#endif

	//img_foreground.copyTo(img_output);
	//img_background.copyTo(img_bgmodel);

	img_foreground.copyTo(_data[0].processing);
}
