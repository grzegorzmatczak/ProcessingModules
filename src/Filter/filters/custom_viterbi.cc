#include "custom_viterbi.h"
#include <QJsonObject>
#include <iostream> 
#include <vector> 

constexpr auto DRON_SIZE_MIN{ "Deep" };
#define DEBUG TRUE

std::string type2str(int type)
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

Filters::Viterbi::Viterbi(QJsonObject const &a_config)
{
  // m_randomGenerator = new QRandomGenerator(m_randSeed);
  // cv::theRNG().state = m_randSeed;
}

void Filters::Viterbi::process(std::vector<_data> &_data)
{
  int m_treck = 2 ;
  int m_range = 16;

  if (m_firstTime) {
    m_firstTime = false;
#if(DEBUG)
    Logger->critical("Viterbi::process() initial data checking...");
    Logger->critical("Viterbi::process() _data[0].processing.cols:{},_data[0].processing.rows:{}",
                  _data[0].processing.cols, _data[0].processing.rows);
    Logger->critical("m_images.size():{}", m_images.size());
#endif
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
  }
  cv::Mat VAL = cv::Mat(m_height, m_width, CV_16UC1, cv::Scalar(0));
#if (DEBUG)
      Logger->critical("m_images.size():{}", m_images.size());
#endif
      cv::Mat clone = _data[0].processing.clone();
      cv::Scalar mean = cv::mean(clone);
      for (int i = 0; i < m_width; i++) {
        for (int j = 0; j < m_height; j++) {
          if (clone.at<unsigned char>(cv::Point(j, i)) > mean[0]) {
            clone.at<unsigned char>(cv::Point(j, i)) = clone.at<unsigned char>(cv::Point(j, i)) - mean[0];
          } else {
            clone.at<unsigned char>(cv::Point(j, i)) = mean[0] - clone.at<unsigned char>(cv::Point(j, i));
          }
        }
      }


  if (m_images.size() > m_range) {
    m_images.pop_front();
    m_images.push_back(clone.clone());

    m_VAL.pop_front();
    m_VAL.push_back(VAL.clone());
  } else {
    m_images.push_back(clone.clone());
    m_VAL.push_back(VAL.clone());
  }

  if (m_VAL.size() > 3) {
    for (int z = 0; z < m_VAL.size(); z++) {
      if (z == 0) {
        //m_VAL[z] = m_images[z].clone();
        for (int i = 0; i < m_width; i++) {
          for (int j = 0; j < m_height; j++) {
            m_VAL[z].at<unsigned short>(cv::Point(j, i)) =
                unsigned short(m_images[z].at<unsigned char>(cv::Point(j, i)));
          }
        }
#if (DEBUG)
        Logger->critical("m_VAL[z] = m_images[z].clone();");
#endif
      } else {
        for (int i = m_treck; i < m_width - m_treck; i++) {
          for (int j = m_treck; j < m_height - m_treck; j++) {
            std::vector<unsigned char> temp;
#if (DEBUG)
            Logger->trace("create temp, z:{}, ixj:{}x{}",z,i,j);
#endif

            for (int ii = -m_treck; ii <= m_treck; ii++) {
              for (int jj = -m_treck; jj <= m_treck; jj++) {
#if (DEBUG)
                //std::string ty = type2str(m_VAL[z - 1].type());
                //printf("Matrix: %s %dx%d \n", ty.c_str(), m_VAL[z - 1].cols, m_VAL[z - 1].rows);
               // Logger->critical("Viterbi::process() m_VAL type:{}", ty);
                //Logger->critical("Viterbi::process() temp.push_back:{}  ",m_VAL[z - 1].at<int>(cv::Point(j + jj, i + ii)));
#endif
                temp.push_back(m_VAL[z - 1].at<unsigned short>(cv::Point(j + jj, i + ii)));

              }
            }

            std::vector<unsigned char>::iterator result;
            result = std::max_element(temp.begin(), temp.end());
            int distance = std::distance(temp.begin(), result);
#if (DEBUG)

            //Logger->trace("Viterbi::process() max:{}", temp[distance]);
#endif
             
            m_VAL[z].at<unsigned short>(cv::Point(j, i)) =
                unsigned short((m_images[z].at<unsigned char>(cv::Point(j, i)) + temp[distance]));
          }
        }
      }
    }
  }


  #if (DEBUG)
  //cv::imshow("input", m_images[m_images.size()-1]);
  cv::Mat m_images_CAT;
  cv::Mat m_images_CAT_thresh;

  for (int i = 0; i < m_images.size(); i++) {
    cv::Mat m_images_temp;
    cv::Mat m_images_temp_color;
   m_images[i].convertTo(m_images_temp, CV_8UC1,1,0);
    //cv::normalize(m_images[i], m_images_temp, 0, 255, cv::NORM_MINMAX, CV_8UC1);



    cv::cvtColor(m_images_temp, m_images_temp_color, cv::COLOR_GRAY2BGR);

    if (i == 0) {
      cv::hconcat(m_images_temp_color, m_images_temp_color, m_images_CAT);
      cv::threshold(m_images_CAT, m_images_CAT_thresh, mean[0]/2, int(255), 0);
    } else {
      cv::hconcat(m_images_CAT, m_images_temp_color, m_images_CAT);
      cv::threshold(m_images_CAT, m_images_CAT_thresh, mean[0]/2, int(255), 0);
    }
  }
  cv::resize(m_images_CAT, m_images_CAT, cv::Size(2800,300), 0, 0,
             cv::INTER_NEAREST);
  cv::imshow("input", m_images_CAT);

  cv::resize(m_images_CAT_thresh, m_images_CAT_thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("input_thresh", m_images_CAT_thresh);


  cv::Mat m_VAL_CAT;
  cv::Mat m_VAL_CAT_Thresh;
  for (int i = 0; i < m_VAL.size(); i++) {

    cv::Mat m_VAL_temp;
    cv::Mat m_VAL_temp_color;
    m_VAL[i].convertTo(m_VAL_temp, CV_8UC1, 1, 0);
    //cv::normalize(m_VAL[i], m_VAL_temp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::cvtColor(m_VAL_temp, m_VAL_temp_color, cv::COLOR_GRAY2BGR);

    if (i==0) {
      cv::hconcat(m_VAL_temp_color, m_VAL_temp_color, m_VAL_CAT);
      cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, mean[0]/2, int(255), 0);
    } else {
      cv::hconcat(m_VAL_CAT, m_VAL_temp_color, m_VAL_CAT);
      cv::threshold(m_VAL_CAT, m_VAL_CAT_Thresh, mean[0]/2, int(255), 0);
    }
   
    

  }
      

  cv::resize(m_VAL_CAT, m_VAL_CAT, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("VAL", m_VAL_CAT);
  cv::resize(m_VAL_CAT_Thresh, m_VAL_CAT_Thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("VAL_Thresh", m_VAL_CAT_Thresh);




  cv::Mat inputResized;
  cv::resize(_data[0].processing, inputResized, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("input", inputResized);


   for (int i = 0; i < m_width; i++) {
    for (int j = 0; j < m_height; j++) {
      // m_VAL[m_VAL.size() - 1].at<int>(cv::Point(j, i)) = int(m_VAL[m_VAL.size() - 1].at<int>(cv::Point(j, i)) / 5);
    }
  }

  cv::Mat VALResized;
  cv::normalize(m_VAL[m_VAL.size() - 1], VALResized, 0, 255, cv::NORM_MINMAX, CV_8UC1);
 // m_VAL[m_VAL.size() - 1].convertTo(VALResized, CV_8UC1, 1, 0);
  cv::resize(VALResized, VALResized, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("VALResized", VALResized);

  cv::Mat InputResized2;
  cv::resize(m_images[m_images.size() - 1], InputResized2, cv::Size(400, 300), 0, 0, cv::INTER_NEAREST);
  cv::imshow("InputABS", InputResized2);

  cv::waitKey(0);
  #endif


}
