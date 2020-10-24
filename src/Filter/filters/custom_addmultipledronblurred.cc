#include "custom_addmultipledronblurred.h"
#include <stdio.h>
#include <QJsonObject>
#include <iostream>

#define DEBUG FALSE
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
constexpr auto IMAGE_OFFSET{ "ImageOffset" };
constexpr auto CONTRAST_OFFSET{ "ContrastOffset" };
constexpr auto DRON_THICKNESS{ "DronThickness" };
constexpr auto GLOBAL_OFFSET{ "GlobalOffset" };
constexpr auto RANDOM_COLOR{ "RandomColor" };

Filters::AddMultipleDronBlurred::AddMultipleDronBlurred(QJsonObject const &a_config)
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
  , m_imageOffset{ a_config[IMAGE_OFFSET].toInt() }
  , m_contrastOffset{ a_config[CONTRAST_OFFSET].toInt() }
  , m_dronThickness{ a_config[DRON_THICKNESS].toInt() }
  , m_globalOffset{ a_config[GLOBAL_OFFSET].toBool() }
  , m_randomColor{ a_config[RANDOM_COLOR].toBool() }
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
  spdlog::info("AddMultipleDronBlurred::AddMultipleDronBlurred()");
  // m_velocityX = 1;
  // m_velocityY = 1;
}

void Filters::AddMultipleDronBlurred::process(std::vector<_data> &_data)
{
  /*
  0 - generated dron with noise on image
  1 - generated dron - so GT
  2 - generated noise
  */
  // qint32 clusterOffset = 6;

  m_iterator++;
  if (m_firstTime) {
    m_firstTime = false;
    Logger->trace("AddDron::AddMultipleDronBlurred() initial data OK");
    Logger->trace("AddDron::AddMultipleDronBlurred() _data[0].processing.cols:{},_data[0].processing.rows:{}",
                  _data[0].processing.cols, _data[0].processing.rows);
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
    qint32 deltaX = 0;
    qint32 deltaY = 0;
    while (true) {
      // Logger->trace("AddDron::AddDron() deltaX:{}, deltaY:{}", deltaX, deltaY);
      if ((deltaX + m_clusterWidth) <= m_width) {
        qint32 clusterOffset = m_randomGenerator->bounded(1, 2);
        m_clusterOffset.push_back(clusterOffset);
        m_X.push_back((m_clusterWidth + clusterOffset) / 2);
        m_Y.push_back((m_clusterHeight + clusterOffset) / 2);
        m_oldX.push_back((m_clusterWidth + clusterOffset) / 2);
        m_oldY.push_back((m_clusterHeight + clusterOffset) / 2);
        struct boundsBlurred tmp {
          0, (m_clusterWidth + clusterOffset), 0, (m_clusterHeight + clusterOffset)
        };
        m_bounds.push_back(tmp);
        m_velocityX.push_back(1);
        m_velocityY.push_back(1);
        m_dronSize.push_back(m_randomGenerator->bounded(m_sizeMin, m_sizeMax + 1));
        m_markerTypeVec.push_back(m_randomGenerator->bounded(0, m_markerType));
        deltaX += m_clusterWidth;
      } else if ((deltaX + m_clusterWidth) > m_width && (deltaY + m_clusterHeight) <= m_height) {
        deltaX = 0;
        deltaY += m_clusterHeight;
      } else if ((deltaX + m_clusterWidth) > m_width && (deltaY + m_clusterHeight) > m_height) {
        break;
      }
    }
  }

  Logger->trace("AddDron::AddMultipleDronBlurred() m_X size:{}", m_X.size());
  // cv::Mat tempImage = _data[0].processing.clone;
  cv::Mat mark = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

  for (int i = 0; i < m_X.size(); i++) {
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
      m_markerTypeVec[i] = m_randomGenerator->bounded(0, m_markerType);
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
    checkBoundies(m_imageOffset, m_X[i], m_Y[i], m_bounds[i]);
    //Logger->info("AddDron::AddMultipleDronBlurred() m_X[{}]:{}", i, m_X[i]);
  }
  Logger->info("AddDron::AddMultipleDronBlurred() drawMarker");
  cv::Mat clone = _data[0].processing.clone();

  qint32 deltaX = 0;
  qint32 deltaY = 0;
  qint32 i = 0;
  while (true) {
    if ((deltaY + m_clusterHeight) >= m_height) {
      break;
    } else if ((deltaX + m_clusterWidth) >= m_width && (deltaY + m_clusterHeight) >= m_height) {
      break;
    } else if ((deltaX + m_clusterWidth) > m_width && (deltaY + m_clusterHeight) <= m_height) {
      deltaX = 0;
      deltaY += m_clusterHeight;
    } else if ((deltaX + m_clusterWidth) <= m_width) {
      cv::Mat mask((m_clusterHeight + m_clusterOffset[i]), (m_clusterWidth + m_clusterOffset[i]), CV_8UC1,
                   cv::Scalar(0));
      if (m_randomColor) {
          //TODO: fix that:!
        //m_color = m_randomGenerator->bounded(0, 255);
        m_color = 255;
      }

      cv::drawMarker(mask, cv::Point(m_X[0], m_Y[0]), cv::Scalar(m_color), m_markerTypeVec[i], m_dronSize[i],
                     m_dronThickness, 8);

      

      //mask = cv::getRotationMatrix2D(pc, rotate, 1.0);
#if (DEBUG)
      std::cout << "mask=" << std::endl << mask << std::endl;
      
      Logger->info("m_clusterOffset[i]:{}", m_clusterOffset[i]);
      Logger->info("m_dronSize[i]:{}", m_dronSize[i]);
      Logger->info("m_markerTypeVec[i]:{}", m_markerTypeVec[i]);
      Logger->info("m_dronThickness:{}", m_dronThickness);
      Logger->info("m_color:{}", m_color);

#endif
      cv::Mat maskResize;
      cv::resize(mask, maskResize, cv::Size(m_clusterWidth, m_clusterHeight));


      cv::Point2f center((maskResize.cols - 1) / 2.0, (maskResize.rows - 1) / 2.0);
      int rotate = m_randomGenerator->bounded(1, 90);
      cv::Mat r = cv::getRotationMatrix2D(center, rotate, 1.0);
      cv::Mat maskRotate;
      cv::warpAffine(maskResize, maskRotate, r, cv::Size(maskResize.cols, maskResize.rows));

#if (DEBUG)
      std::cout << "maskResize=" << std::endl << maskResize << std::endl;
      std::cout << "maskRotate=" << std::endl << maskRotate << std::endl;
#endif
      cv::Rect rect(deltaX, deltaY, maskRotate.cols, maskRotate.rows);
      maskRotate.copyTo(mark(rect));
      cv::Mat cleanROI = clone(rect);

      cv::Scalar m = cv::mean(cleanROI);
      bool up_down = m_randomGenerator->bounded(0, 2);
      double delta(1.0);
      double offset{ 1.0 };
      cv::Mat maskResizeOffset;
#if (DEBUG)
      cv::imshow("before:", maskResize);
      cv::imshow("maskRotate:", maskRotate);
      Logger->info("m_contrastOffset:{}", m_contrastOffset);
#endif
      if (up_down) {
        
        delta = (255.0 - m[0]);
        offset = delta * (m_contrastOffset / 100.0);
        maskRotate.convertTo(maskRotate, -1, (delta + offset) / 255.0, 0);
#if (DEBUG)
        Logger->info("up: delta:{}", delta);
        Logger->info("up: offset:{}", offset);
        cv::imshow("up: middle:", maskRotate);
#endif
        cv::threshold(maskRotate, maskResizeOffset, 1, offset, 0);
        cv::add(cleanROI, maskRotate, cleanROI);
        cv::add(cleanROI, maskResizeOffset, cleanROI);
      } else {
        delta = (m[0]) ;
        offset = delta * (m_contrastOffset / 100.0);
        maskRotate.convertTo(maskRotate, -1, (delta - offset) / 255.0, 0);
#if (DEBUG)
        Logger->info("down: delta:{}", delta);
        Logger->info("down: offset:{}", offset);
        cv::imshow("down: middle:", maskRotate);
#endif
        cv::threshold(maskRotate, maskResizeOffset, 1, offset, 0);
        cv::subtract(cleanROI, maskRotate, cleanROI);
        cv::subtract(cleanROI, maskResizeOffset, cleanROI);
      }
#if (DEBUG)
      cv::imshow("cleanROI:", cleanROI);
      cv::imshow("maskRotate:", maskRotate);
      cv::imshow("maskResizeOffset:", maskResizeOffset);
      cv::waitKey(0);
#endif
      cleanROI.copyTo(clone(rect));
      deltaX += m_clusterWidth;
      i++;
    }
  }
  struct _data dataTemp;
  if (m_iterator <= m_startGT) // create GT:
  {
    cv::Mat tempImageGT = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(m_unknownGTColor));
    dataTemp.processing = tempImageGT.clone();
  } // Create unknown GT Color:
  else if (m_iterator > m_startGT) {
    dataTemp.processing = mark.clone();
  }
  _data.push_back(dataTemp);

  // 2:
  cv::Mat noise_image(_data[0].processing.size(), CV_16SC1);
  cv::randn(noise_image, cv::Scalar::all(0.0), cv::Scalar::all(m_noise_double));
  struct _data dataTempGT;
  dataTempGT.processing = noise_image.clone();
  _data.push_back(dataTempGT);

  // 0:
  cv::Mat temp_image;
  cv::Mat mark2;
  clone.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(_data[0].processing, _data[0].processing.type());

  Logger->debug("AddDron::AddMultipleDronBlurred() [{}] cols:{},{},{}", m_iterator, _data[0].processing.cols,
                _data[1].processing.rows,
    _data[2].processing.channels());

  for (int i = 0; i < m_X.size(); i++) {
    m_oldX[i] = m_X[i];
    m_oldY[i] = m_Y[i];
  }

  Logger->trace("AddDron::AddMultipleDronBlurred() done");
}

void Filters::AddMultipleDronBlurred::checkBoundies(const qint32 &offset, qint32 &x, qint32 &y,
                                                    const struct boundsBlurred &b)
{
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

void Filters::AddMultipleDronBlurred::addGaussianNoise(cv::Mat &image, double average, double standard_deviation,
                                                       cv::Mat &noise)
{
  cv::Mat noise_image(image.size(), CV_16SC1);
  randn(noise_image, cv::Scalar::all(average), cv::Scalar::all(standard_deviation));
  cv::Mat temp_image;
  image.convertTo(temp_image, CV_16SC1);
  addWeighted(temp_image, 1.0, noise_image, 1.0, 0.0, temp_image);
  temp_image.convertTo(image, image.type());
  noise_image.convertTo(noise, noise.type());
}
