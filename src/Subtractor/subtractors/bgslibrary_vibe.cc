#include "bgslibrary_vibe.h"

#include <QJsonObject>

//#define DEBUG

constexpr auto NUMBER_OF_SAMPLES{ "NumberOfSamples" };
constexpr auto MATCHING_THRESHOLD{ "MatchingThreshold" };
constexpr auto MATCHING_NUMBER{ "MatchingNumber" };
constexpr auto UPDATE_FACTOR{ "UpdateFactor" };


Subtractors::ViBe::ViBe(QJsonObject const &a_config)
	: m_numberOfSamples{ a_config[NUMBER_OF_SAMPLES].toInt() }
	, m_matchingThreshold{ a_config[MATCHING_THRESHOLD].toInt() }
	, m_matchingNumber{ a_config[MATCHING_NUMBER].toInt() }
	, m_updateFactor{ a_config[UPDATE_FACTOR].toInt() }
	, model(nullptr)
	, m_firstTime(true)
{
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::ViBe()");
	#endif
	model = vibe::libvibeModel_Sequential_New();
	
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::ViBe() model create done");
	#endif
}

Subtractors::ViBe::~ViBe()
{
	vibe::libvibeModel_Sequential_Free(model);
}

void Subtractors::ViBe::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() ");
	#endif

	if (_data[0].processing.empty()) 
	{
		//Logger->error("Subtractors::ViBe::process() _data[0].processing.empty()");
	}

	if (m_firstTime)
	{
		/* Create a buffer for the output image. */
		// img_output = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);

		/* Initialization of the ViBe model. */
		#ifdef DEBUG
		//Logger->debug("Subtractors::ViBe::process() m_firstTime:");
		#endif
	 // vibe::libvibeModel_Sequential_AllocInit_8u_C3R(model, _data[0].processing.data, _data[0].processing.cols,
		//                                               _data[0].processing.rows);
		vibe::libvibeModel_Sequential_AllocInit_8u_C1R(model, _data[0].processing.data, _data[0].processing.cols,
																									 _data[0].processing.rows);

		/* Sets default model values. */
		//vibe::libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
		vibe::libvibeModel_Sequential_SetMatchingThreshold(model, m_matchingThreshold);
		vibe::libvibeModel_Sequential_SetMatchingNumber(model, m_matchingNumber);
		vibe::libvibeModel_Sequential_SetUpdateFactor(model, m_updateFactor);
		#ifdef DEBUG
		//Logger->debug("Subtractors::ViBe::process() m_firstTime: setting done");
		#endif
	}
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() struct _data data:");
	#endif
	struct _data data;
	cv::Mat tempImage=cv::Mat(_data[0].processing.rows, _data[0].processing.cols, CV_8UC1, cv::Scalar(0));
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() libvibeModel_Sequential_Segmentation_8u_C3R:");
	#endif
	vibe::libvibeModel_Sequential_Segmentation_8u_C1R(model, _data[0].processing.data, tempImage.data);
	// vibe::libvibeModel_Sequential_Update_8u_C3R(model, model_img_input.data, img_output.data);
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() libvibeModel_Sequential_Update_8u_C3R:");
	#endif
	vibe::libvibeModel_Sequential_Update_8u_C1R(model, _data[0].processing.data, tempImage.data);
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() _data.push_back(data):");
	#endif

	_data[0].processing = tempImage.clone();

	m_firstTime = false;
	#ifdef DEBUG
	//Logger->debug("Subtractors::ViBe::process() done");
	#endif
}
