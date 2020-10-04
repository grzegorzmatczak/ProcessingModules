#include "custom_adddron.h"
#include <QJsonObject>
/*
*   MarkerType:
    MARKER_CROSS = 0,            //!< A crosshair marker shape
    MARKER_TILTED_CROSS = 1, //!< A 45 degree tilted crosshair marker shape
    MARKER_STAR = 2,         //!< A star marker shape, combination of cross and tilted cross
    MARKER_DIAMOND = 3,      //!< A diamond marker shape
    MARKER_SQUARE = 4,       //!< A square marker shape
    MARKER_TRIANGLE_UP = 5,  //!< An upwards pointing triangle marker shape
    MARKER_TRIANGLE_DOWN = 6 //!< A downwards pointing triangle marker shape
*/
constexpr auto DRON_SIZE_MIN{ "SizeMin" };
constexpr auto DRON_SIZE_MAX{ "SizeMax" };
constexpr auto DRON_COLOR{ "Color" };
constexpr auto DRON_ROTATE{ "Rotate" };
constexpr auto DRON_VELOCITY_MIN{ "VelocityMin" };
constexpr auto DRON_VELOCITY_MAX{ "VelocityMax" };
constexpr auto DRON_PROB_OF_CHANGE_SIZE{ "ProbabilityOfChangeSize" };
constexpr auto DRON_PROB_OF_CHANGE_Velocity{ "ProbabilityOfChangeVelocity" };
constexpr auto DRON_PROB_OF_ROTATE{ "ProbabilityOfRotate" };
constexpr auto DRON_MARKER_TYPE{ "MarkerType" };
constexpr auto DRON_START_GT{ "StartGT" };
constexpr auto DRON_UNKNOWN_GT_COLOR{ "UnknownGTColor" };
constexpr auto DRON_RANDSEED{ "RandSeed" };
constexpr auto DRON_NOISE{ "Noise" };

Filters::AddDron::AddDron(QJsonObject const &a_config)
  : m_sizeMin{ a_config[DRON_SIZE_MIN].toInt() }
  , m_sizeMax{ a_config[DRON_SIZE_MAX].toInt() }
  , m_color{ a_config[DRON_COLOR].toInt() }
  , m_rotate{ a_config[DRON_ROTATE].toInt() }
  , m_velocityMin{ a_config[DRON_VELOCITY_MIN].toInt() }
  , m_velocityMax{ a_config[DRON_VELOCITY_MAX].toInt() }
  , m_probabilityOfChangeSize{ a_config[DRON_PROB_OF_CHANGE_SIZE].toDouble() }
  , m_probabilityOfChangeVelocity{ a_config[DRON_PROB_OF_CHANGE_Velocity].toDouble() }
  , m_probabilityOfRotate{ a_config[DRON_PROB_OF_ROTATE].toDouble() }
  , m_markerType{ a_config[DRON_MARKER_TYPE].toInt() }
  , m_startGT{ a_config[DRON_START_GT].toInt() }
  , m_unknownGTColor{ a_config[DRON_UNKNOWN_GT_COLOR].toInt() }
  , m_randSeed{ a_config[DRON_RANDSEED].toInt() }
  , m_noise_int{ a_config[DRON_NOISE].toInt() }
  , m_firstTime{true}
{
  // Init randseed's:
  m_randomGenerator = new QRandomGenerator(m_randSeed);
  cv::theRNG().state = m_randSeed;
  m_noise_double = m_noise_int / 1.0;
  m_oldRandX = 1;
  m_oldRandY = 1;
  m_randX = 1;
  m_randY = 1;
  dronVelocity = 3;
  offset = 5;
  m_iterator = 0;
  //spdlog::debug("AddDron::AddDron()");
  m_velocityX = 1;
  m_velocityY = 1;
}

void Filters::AddDron::process(std::vector<_data> &_data)
{
  /*
  0 - generated dron with noise on image
  1 - generated dron - so GT
  2 - generated noise 
  */


  m_iterator++;
  if (m_firstTime) {
    m_firstTime = false;
    spdlog::trace("AddDron::AddDron() initial data");
   // spdlog::trace("AddDron::AddDron() _data[0].processing.cols:{},_data[0].processing.rows:{}",
   //               _data[0].processing.cols, _data[0].processing.rows);
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;

    m_randX = m_width / 2;
    m_randY = m_height / 2;
    m_oldRandX = m_randX;
    m_oldRandY = m_randY;
  }
  cv::Mat mask(m_height, m_width, CV_8UC1, cv::Scalar(255));
  //cv::Mat tempImage = _data[0].processing.clone;
  cv::Mat mark = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(255));

  double _chanceOfChangeVelocity = m_randomGenerator->bounded(0, 100) / 100.0;
  if (_chanceOfChangeVelocity < m_probabilityOfChangeVelocity) {
    m_velocityX = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
    m_velocityY = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
    double tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
    if (tempProb < 0.5) {
      m_velocityX = -m_velocityX;
    }
    tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
    if (tempProb < 0.5) {
      m_velocityY = -m_velocityY;
    }
  }
  // lottery change size:
  double _chanceOfChangeSize = m_randomGenerator->bounded(0, 100) / 100.0;
  if (_chanceOfChangeSize < m_probabilityOfChangeSize) {
    dronSize = m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1);
  }
   
 double _chanceOfRotate = m_randomGenerator->bounded(0, 100) / 100.0;
  if (_chanceOfRotate < m_probabilityOfRotate) {
   m_markerType = m_randomGenerator->bounded(0, 2);
  }

  m_randX = m_oldRandX + m_velocityX;
  m_randY = m_oldRandY + m_velocityY;
  
  checkBoundies(offset);

  cv::drawMarker(mark, cv::Point(m_randX, m_randY), cv::Scalar(m_color), 
      m_markerType, dronSize, 2,8);

  struct _data dataTemp;
  //spdlog::debug("AddDron::AddDron() m_iterator:{} m_startGT:{}", m_iterator, m_startGT);
  //spdlog::debug("AddDron::AddDron() m_height:{} m_width:{}", m_height, m_width);
  if (m_iterator <= m_startGT) // create GT:
  {
    cv::Mat tempImageGT = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(m_unknownGTColor));
    dataTemp.processing = tempImageGT.clone();
  }  // Create unknown GT Color:
  else if (m_iterator > m_startGT)
    {
    //_data[1].processing = tempImage.clone();
    dataTemp.processing = mark.clone();
  }
  _data.push_back(dataTemp);
  //spdlog::debug("AddDron::AddDron() _data.size()", _data.size());
  //spdlog::debug("AddDron::AddDron() m_noise_double.", _data.size());

  //2:
  cv::Mat noise_image(_data[0].processing.size(), CV_16SC1);
  cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(m_noise_double));
  //_data[2].processing;
  struct _data dataTempGT;
  dataTempGT.processing = noise_image.clone();
  _data.push_back(dataTempGT);
  //spdlog::debug("AddDron::AddDron() _data.size()", _data.size());

  //0:
  cv::Mat temp_image;
  mark = _data[0].processing.clone();
  cv::drawMarker(mark, cv::Point(m_randX, m_randY), cv::Scalar(m_color), m_markerType, dronSize, 2, 8);
  //spdlog::debug("AddDron::AddDron() after clone ");
  mark.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(_data[0].processing, _data[0].processing.type());

  //spdlog::debug("AddDron::AddDron() cols:{},{},{}", _data[0].processing.cols, _data[1].processing.cols,
  //               _data[2].processing.cols);

  m_oldRandX = m_randX;
  m_oldRandY = m_randY;

  spdlog::trace("AddDron::AddDron() done");

}

void Filters::AddDron::checkBoundies(qint32 offset)
{
  //spdlog::trace("AddDron::AddDron() checkBoundies");
  if (m_randX < offset) {
    m_randX = offset;
  }
  if (m_randX > (m_width - offset)) {
    m_randX = (m_width - offset);
  }

  if (m_randY < offset) {
    m_randY = offset;
  }
  if (m_randY > (m_height - offset)) {
    m_randY = (m_height - offset);
  }
}

void Filters::AddDron::addGaussianNoise(cv::Mat &image, double average, double standard_deviation, cv::Mat &noise)
{
  //spdlog::trace("AddDron::AddDron() addGaussianNoise");
  cv::Mat noise_image(image.size(), CV_16SC1);
  randn(noise_image, cv::Scalar::all(average), cv::Scalar::all(standard_deviation));
  cv::Mat temp_image;
  image.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(image, image.type());
  noise_image.convertTo(noise, noise.type());
}
