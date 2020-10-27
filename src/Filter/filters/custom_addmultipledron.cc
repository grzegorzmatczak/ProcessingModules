#include "custom_addmultipledron.h"
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
constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto OFFSET{ "Offset" };
constexpr auto DRON_THICKNESS{ "DronThickness" };
constexpr auto GLOBAL_OFFSET{ "GlobalOffset" };

Filters::AddMultipleDron::AddMultipleDron(QJsonObject const &a_config)
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
  , m_firstTime{ true }
  , m_clusterWidth{ a_config[CLUSTER_WIDTH].toInt() }
  , m_clusterHeight{ a_config[CLUSTER_HEIGHT].toInt() }
  , m_offset{ a_config[OFFSET].toInt() }
  , m_dronThickness{ a_config[DRON_THICKNESS].toInt() }
  , m_globalOffset{ a_config[GLOBAL_OFFSET].toBool() }
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
  //offset = 5;
  m_iterator = 0;
  spdlog::debug("AddMultipleDron::AddMultipleDron()");
  //m_velocityX = 1;
  //m_velocityY = 1;
}

void Filters::AddMultipleDron::process(std::vector<_data> &_data)
{
  /*
  0 - generated dron with noise on image
  1 - generated dron - so GT
  2 - generated noise
  */
  m_iterator++;
  if (m_firstTime) {
    m_firstTime = false;
    //Logger->trace("AddDron::AddDron() initial data OK");
    //Logger->trace("AddDron::AddDron() _data[0].processing.cols:{},_data[0].processing.rows:{}",
    //              _data[0].processing.cols, _data[0].processing.rows);
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
    qint32 deltaX = 0;
    qint32 deltaY = 0;
    while (true) {
      //Logger->trace("AddDron::AddDron() deltaX:{}, deltaY:{}", deltaX, deltaY);
      if ((deltaX + m_clusterWidth)<= m_width) {
        m_X.push_back(deltaX + m_clusterWidth / 2);
        m_Y.push_back(deltaY + m_clusterHeight / 2);
        m_oldX.push_back(deltaX + m_clusterWidth / 2);
        m_oldY.push_back(deltaY + m_clusterHeight / 2);
        struct bounds tmp {
          deltaX, deltaX + m_clusterWidth, deltaY, deltaY + m_clusterHeight
        };
        m_bounds.push_back(tmp);
        m_velocityX.push_back(1);
        m_velocityY.push_back(1);
        m_dronSize.push_back(1);
        m_markerType.push_back(1);
        deltaX += m_clusterWidth;
      } else if ((deltaX + m_clusterWidth) > m_width && (deltaY + m_clusterHeight) <= m_height) {
        deltaX = 0;
        deltaY += m_clusterHeight;
      } else if ((deltaX + m_clusterWidth) > m_width && (deltaY + m_clusterHeight) > m_height) {
        break;
      }
    }
  }

  //Logger->trace("AddDron::AddDron() m_X size:{}", m_X.size());

  cv::Mat mask(m_height, m_width, CV_8UC1, cv::Scalar(0));
  // cv::Mat tempImage = _data[0].processing.clone;
  cv::Mat mark = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

  for (int i = 0 ; i < m_X.size(); i++)
  {
    double _chanceOfChangeVelocity = m_randomGenerator->bounded(0, 100) / 100.0;
    if (_chanceOfChangeVelocity < m_probabilityOfChangeVelocity) {
      m_velocityX[i] = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
      m_velocityY[i] = m_randomGenerator->bounded(m_velocityMin, m_velocityMax + 1);
      double tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
      if (tempProb < 0.5) {
        m_velocityX[i] = -m_velocityX[i];
      }
      tempProb = m_randomGenerator->bounded(0, 10) / 10.0;
      if (tempProb < 0.5) {
        m_velocityY[i] = -m_velocityY[i];
      }
    }
  }
  
  // lottery change size:
  for (int i = 0; i < m_X.size(); i++) {
    double _chanceOfChangeSize = m_randomGenerator->bounded(0, 100) / 100.0;
    if (_chanceOfChangeSize < m_probabilityOfChangeSize) {
      m_dronSize[i] = m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1);
    }
  }

   // lottery rotate:
   for (int i = 0; i < m_X.size(); i++) {
    double _chanceOfRotate = m_randomGenerator->bounded(0, 100) / 100.0;
    if (_chanceOfRotate < m_probabilityOfRotate) {
      m_markerType[i] = m_randomGenerator->bounded(0, 2);
    }
  }
   // update x,y
  if (m_globalOffset) {
    for (int i = 0; i < m_X.size(); i++) {
      m_X[i] = m_oldX[i] + m_velocityX[0];
      m_Y[i] = m_oldY[i] + m_velocityY[0];
    }
  } else {
    for (int i = 0; i < m_X.size(); i++) {
      m_X[i] = m_oldX[i] + m_velocityX[i];
      m_Y[i] = m_oldY[i] + m_velocityY[i];
    }
  }
   
   // checkBoundies
   for (int i = 0; i < m_X.size(); i++) {
     checkBoundies(m_offset, m_X[i], m_Y[i], m_bounds[i]);
   }
   //Logger->trace("AddDron::AddDron() drawMarker");
   for (int i = 0; i < m_X.size(); i++) {
     cv::drawMarker(mark, cv::Point(m_X[i], m_Y[i]), cv::Scalar(m_color), m_markerType[i], m_dronSize[i],
                    m_dronThickness, 8);
   }
  struct _data dataTemp;
  if (m_iterator <= m_startGT) // create GT:
  {
    cv::Mat tempImageGT = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(m_unknownGTColor));
    dataTemp.processing = tempImageGT.clone();
  } // Create unknown GT Color:
  else if (m_iterator > m_startGT) {
    //_data[1].processing = tempImage.clone();
    dataTemp.processing = mark.clone();
  }
  _data.push_back(dataTemp);
  // Logger->debug("AddDron::AddDron() _data.size()", _data.size());
  // Logger->debug("AddDron::AddDron() m_noise_double.", _data.size());

  // 2:
  cv::Mat noise_image(_data[0].processing.size(), CV_16SC1);
  cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(m_noise_double));
  //_data[2].processing;
  struct _data dataTempGT;
  dataTempGT.processing = noise_image.clone();
  _data.push_back(dataTempGT);
  // Logger->debug("AddDron::AddDron() _data.size()", _data.size());

  // 0:
  cv::Mat temp_image;
  mark = _data[0].processing.clone();
  for (int i = 0; i < m_X.size(); i++) {
    cv::drawMarker(mark, cv::Point(m_X[i], m_Y[i]), cv::Scalar(255 - m_color), m_markerType[i], m_dronSize[i],
                   m_dronThickness, 8);
  }
  // Logger->debug("AddDron::AddDron() after clone ");
  mark.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(_data[0].processing, _data[0].processing.type());

  // Logger->debug("AddDron::AddDron() cols:{},{},{}", _data[0].processing.cols, _data[1].processing.cols,
  //               _data[2].processing.cols);

  //m_oldRandX = m_randX;
  //m_oldRandY = m_randY;
  for (int i = 0; i < m_X.size(); i++) {
    m_oldX[i] = m_X[i];
    m_oldY[i] = m_Y[i];
  }


  //Logger->trace("AddDron::AddDron() done");
}

void Filters::AddMultipleDron::checkBoundies(const qint32 &offset, qint32 &x, qint32 &y,
                                             const struct bounds &b)
{
  //Logger->trace("AddDron::AddDron() checkBoundies");
  if (x < b.x1 + offset) {
    x = b.x1 + offset;
  }
  if (y < b.y1 + offset) {
    y = b.y1 + offset;
  }

  if (y > (b.y2 -offset)) {
    y = (b.y2 - offset);
  }
  if (x > (b.x2 - offset)) {
    x = (b.x2 - offset);
  }
}

void Filters::AddMultipleDron::addGaussianNoise(cv::Mat &image, double average, double standard_deviation,
                                                cv::Mat &noise)
{
  // Logger->trace("AddDron::AddDron() addGaussianNoise");
  cv::Mat noise_image(image.size(), CV_16SC1);
  randn(noise_image, cv::Scalar::all(average), cv::Scalar::all(standard_deviation));
  cv::Mat temp_image;
  image.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(image, image.type());
  noise_image.convertTo(noise, noise.type());
}
