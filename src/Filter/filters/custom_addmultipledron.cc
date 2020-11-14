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

std::string type2strDron(int type)
{
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch (depth) {
    case CV_8U: r = "8U"; break;
    case CV_8S: r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default: r = "User"; break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}

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
  , m_singleMarkerType{ a_config[DRON_MARKER_TYPE].toInt() }
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
  // offset = 5;
  m_iterator = 0;
  spdlog::debug("AddMultipleDron::AddMultipleDron()");
  // m_velocityX = 1;
  // m_velocityY = 1;
}

void Filters::AddMultipleDron::process(std::vector<_data> &_data)
{
  /*
  0 - generated dron with noise on image
  1 - generated dron - GT
  2 - generated noise
  */

  int m_noiseStart = 1;
  int m_noiseStop = 50;
  int m_noiseDelta = 3;
  int m_contrastStart = 1;
  int m_contrastStop = 99;
  int m_contrastDelta = 5;
  bool color = false;

  m_iterator++;
  if (m_firstTime) {
    m_firstTime = false;

    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
    qint32 deltaX = 0;
    qint32 deltaY = 0;

    m_X = (deltaX + m_clusterWidth / 2);
    m_Y = (deltaY + m_clusterHeight / 2);
    m_oldX = (deltaX + m_clusterWidth / 2);
    m_oldY = (deltaY + m_clusterHeight / 2);
    struct bounds tmp {
      deltaX, deltaX + m_clusterWidth, deltaY, deltaY + m_clusterHeight
    };
    m_bounds = (tmp);
    m_velocityX = (1);
    m_velocityY = (1);
    m_dronSize = (m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1));
    m_markerType = (m_singleMarkerType);
  }

  // Ensure that roi is in image:
  cv::Mat clone;
  if (m_clusterWidth > m_width || m_clusterHeight > m_height) {
    cv::resize(_data[0].processing, clone, cv::Size(m_width, m_height));
  } else {
    clone = _data[0].processing(cv::Rect(0, 0, m_clusterWidth, m_clusterHeight));
  }
  cv::Scalar mean = cv::mean(clone);
  cv::Mat output(m_clusterHeight * 20, m_clusterWidth * 20, CV_8UC1, cv::Scalar(0));

  cv::Mat cleanDron(m_clusterHeight, m_clusterWidth, CV_8UC1, cv::Scalar(0));
  cv::drawMarker(cleanDron, cv::Point(m_X, m_Y), 255, m_singleMarkerType,
                 m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1), m_dronThickness, 8);

  std::vector<std::vector<cv::Mat>> images;

  for (int i = m_noiseStart; i <= m_noiseStop; i += m_noiseDelta) {
    std::vector<cv::Mat> image;
    for (int j = m_contrastStart; j <= m_contrastStop; j += m_contrastDelta) {
      cv::Mat tempClone = clone.clone();
      cv::Mat tempDron = cleanDron.clone();
      int contrast = int(255.0 * (j / 100.0));
      cv::Mat tempDronContrast;
      cv::threshold(tempDron, tempDronContrast, 1, contrast, 0);
      // Logger->debug("noise:{},contrast:{} Calculated contrast:{}",i,j, contrast);
      // mean[0]
      int contrastImage;
      int delta;
      cv::Mat tempDronContrastImage;

      if (color) {
        delta = 255 - mean[0];
        contrastImage = delta * (j / 100.0);

        cv::threshold(tempDron, tempDronContrastImage, 1, contrastImage, 0);
        cv::add(tempClone, tempDronContrastImage, tempDronContrastImage);
      } else {
        delta = mean[0];
        contrastImage = delta * (j / 100.0);

        cv::threshold(tempDron, tempDronContrastImage, 1, contrastImage, 0);
        cv::subtract(tempClone, tempDronContrastImage, tempDronContrastImage);
      }
      // Logger->debug("delta:{}", delta);
      // Logger->debug("contrastImage:{}", contrastImage);
      // Logger->debug("");

      cv::Mat noise_image(tempDronContrastImage.size(), CV_16SC1);
      double m_noise_double = i / 1.0;
      cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(m_noise_double));
      cv::Mat temp_image;
      cv::Mat mark2;
      tempDronContrastImage.convertTo(temp_image, CV_16SC1);
      addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
      temp_image.convertTo(tempDronContrastImage, tempDronContrastImage.type());

      image.push_back(tempDronContrastImage);
    }
    images.push_back(image);
  }

  cv::Mat all;
  std::vector<cv::Mat> part;
  //Logger->debug("images.size():{}", images.size());
  for (int i = 0; i < images.size(); i++) {
    cv::Mat partSingle;
    //Logger->debug("images[{}].size():{}", i, images[i].size());
    for (int j = 1; j < images[i].size(); j++) {
      //Logger->debug("images[{}][{}]", i, j);
      std::string type = type2strDron(images[i][j].type());
     // Logger->debug("images[{}][{}].type():{}", i, j, type);
      //Logger->debug("images[{}][{}]:{}x{}", i, j, images[i][j].cols, images[i][j].rows);
      // cv::cvtColor(images[i][j], images[i][j], cv::COLOR_GRAY2BGR);
      //Logger->debug("images[{}][{}].type():{}", i, j, type);
      if (j == 1) {
        cv::hconcat(images[i][0], images[i][1], partSingle);
      } else {
        cv::hconcat(partSingle, images[i][j], partSingle);
      }
    }
    part.push_back(partSingle);

    if (part.size() == 2) {
      cv::vconcat(part[0], part[1], all);
    } else if (part.size() > 2) {
      cv::vconcat(all, partSingle, all);
    }
  }

  cv::imshow("all:", all);
  cv::waitKey(0);

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
    m_dronSize = m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1);
  }

  // lottery rotate:
  double _chanceOfRotate = m_randomGenerator->bounded(0, 100) / 100.0;
  if (_chanceOfRotate < m_probabilityOfRotate) {
    m_markerType = m_randomGenerator->bounded(0, 2);
  }

  // update x,y
  m_X = m_oldX + m_velocityX;
  m_Y = m_oldY + m_velocityY;

  // checkBoundies
  checkBoundies(m_offset, m_X, m_Y, m_bounds);

  m_oldX = m_X;
  m_oldY = m_Y;


  //0:
  _data[0].processing = all.clone();
  struct _data data;
  data.processing = _data[0].processing.clone();
  _data.push_back(data);

  struct _data data2;
  data2.processing = _data[0].processing.clone();
  _data.push_back(data2);
}

void Filters::AddMultipleDron::checkBoundies(const qint32 &offset, qint32 &x, qint32 &y, const struct bounds &b)
{
  // Logger->trace("AddDron::AddDron() checkBoundies");
  if (x < b.x1 + offset) {
    x = b.x1 + offset;
  }
  if (y < b.y1 + offset) {
    y = b.y1 + offset;
  }

  if (y > (b.y2 - offset)) {
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
