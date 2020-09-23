#include "custom_vibe.h"
#include <QJsonObject>

constexpr auto NUMBER_OF_SAMPLES{ "NumberOfSamples" };
constexpr auto MATCHING_THRESHOLD{ "MatchingThreshold" };
constexpr auto MATCHING_NUMBER{ "MatchingNumber" };
constexpr auto UPDATE_FACTOR{ "UpdateFactor" };

Subtractors::ViBe::ViBe(QJsonObject const &a_config)
  : m_numberOfSamples{ a_config[NUMBER_OF_SAMPLES].toInt() }
  , m_matchingThreshold{ a_config[MATCHING_THRESHOLD].toInt() }
  , m_matchingNumber{ a_config[MATCHING_NUMBER].toInt() }
  , m_updateFactor{ a_config[UPDATE_FACTOR].toInt() }
  , model{nullptr}
{
  spdlog::debug("Subtractors::ViBe::ViBe()");
  model = vibe::libvibeModel_Sequential_New();
  spdlog::debug("Subtractors::ViBe::ViBe() model create done");
}

void Subtractors::ViBe::process(std::vector<_data> &_data)
{
  spdlog::debug("Subtractors::ViBe::process() ");
  //vibe
  if (_data[0].processing.empty()) 
      spdlog::error("Subtractors::ViBe::process() _data[0].processing.empty()");
	  //return;
  spdlog::debug("Subtractors::ViBe::process() m_firstTime ", m_firstTime);
  if (m_firstTime) {
    /* Create a buffer for the output image. */
    // img_output = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);

    /* Initialization of the ViBe model. */
    spdlog::debug("Subtractors::ViBe::process() m_firstTime:");
   // vibe::libvibeModel_Sequential_AllocInit_8u_C3R(model, _data[0].processing.data, _data[0].processing.cols,
    //                                               _data[0].processing.rows);
    vibe::libvibeModel_Sequential_AllocInit_8u_C1R(model, _data[0].processing.data, _data[0].processing.cols,
                                                   _data[0].processing.rows);

    /* Sets default model values. */
    // vibe::libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
    vibe::libvibeModel_Sequential_SetMatchingThreshold(model, m_matchingThreshold);
    vibe::libvibeModel_Sequential_SetMatchingNumber(model, m_matchingNumber);
    vibe::libvibeModel_Sequential_SetUpdateFactor(model, m_updateFactor);
    spdlog::debug("Subtractors::ViBe::process() m_firstTime: setting done");
  }
  spdlog::debug("Subtractors::ViBe::process() struct _data data:");
  struct _data data;
  cv::Mat tempImage=
      cv::Mat(_data[0].processing.rows, _data[0].processing.cols, CV_8UC1, cv::Scalar(0));
  spdlog::debug("Subtractors::ViBe::process() libvibeModel_Sequential_Segmentation_8u_C3R:");
  vibe::libvibeModel_Sequential_Segmentation_8u_C1R(model, _data[0].processing.data, tempImage.data);
  // vibe::libvibeModel_Sequential_Update_8u_C3R(model, model_img_input.data, img_output.data);
  spdlog::debug("Subtractors::ViBe::process() libvibeModel_Sequential_Update_8u_C3R:");
  vibe::libvibeModel_Sequential_Update_8u_C1R(model, _data[0].processing.data, tempImage.data);
  spdlog::debug("Subtractors::ViBe::process() _data.push_back(data):");
  data.processing = tempImage.clone();
  _data.push_back(data);

  m_firstTime = false;
  spdlog::debug("Subtractors::ViBe::process() done");
}
