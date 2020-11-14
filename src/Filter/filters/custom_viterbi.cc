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
  m_firstTime = true;
}

void Filters::Viterbi::process(std::vector<_data> &_data)
{
  int m_treck = 1;
  int m_range = 10;
  int clusterWidth = 50;
  int clusterHeight = 50;
  bool m_absFilter = true;

  if (m_firstTime) {
    m_firstTime = false;
#if (DEBUG)
    cv::imshow("input", _data[0].processing);
    Logger->critical("Viterbi::process() _data[0].processing.cols:{},_data[0].processing.rows:{}",
                     _data[0].processing.cols, _data[0].processing.rows);
#endif
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
  }

  cv::Mat clone = _data[0].processing.clone();

  cv::Mat preview;
  cv::cvtColor(_data[0].processing, preview, cv::COLOR_GRAY2BGR);

  /*
  //= _data[0].processing.clone();
  cv::Scalar mean = cv::mean(clone);
  if (m_absFilter) {
    for (int i = 0; i < m_width; i++) {
      for (int j = 0; j < m_height; j++) {
        if (clone.at<unsigned char>(cv::Point(j, i)) > mean[0]) {
          clone.at<unsigned char>(cv::Point(j, i)) = clone.at<unsigned char>(cv::Point(j, i)) - mean[0];
        } else {
          clone.at<unsigned char>(cv::Point(j, i)) = mean[0] - clone.at<unsigned char>(cv::Point(j, i));
        }
      }
    }
  }
  */


  Logger->critical("push to deque:");
  std::vector<cv::Mat> kernels;
  std::vector<cv::Mat> kernelsVAL;
  std::vector<cv::Mat> ViterbiOut;
  std::vector<cv::Point> min;
  std::vector<cv::Point> max;
  std::vector<cv::Point> means;

  int i_shift = 50;
  int j_shift = 50;
  cv::Mat ViterbiOutGlobal = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

  for (int i = 0; i < m_width - clusterWidth; i = i + i_shift) {     // TODO: change to i++
    for (int j = 0; j < m_height - clusterHeight; j = j + j_shift) { // TODO: change to j++
      cv::Rect rect(i, j, clusterWidth, clusterHeight);
      cv::Mat kernel = clone(rect);
      if (m_absFilter) {
       
        cv::Scalar mean = cv::mean(kernel);
        for (int ii = 0; ii < clusterWidth; ii++) {
          for (int jj = 0; jj < clusterHeight; jj++) {
            if (kernel.at<unsigned char>(cv::Point(jj, ii)) > mean[0]) {
              kernel.at<unsigned char>(cv::Point(jj, ii)) = kernel.at<unsigned char>(cv::Point(jj, ii)) - mean[0];
            } else {
              kernel.at<unsigned char>(cv::Point(jj, ii)) = mean[0] - kernel.at<unsigned char>(cv::Point(jj, ii));
            }
          }
        }
        
      } 
      kernels.push_back(kernel.clone());
      cv::Mat VAL = cv::Mat(clusterHeight, clusterWidth, CV_16UC1, cv::Scalar(0));
      kernelsVAL.push_back(VAL.clone());
    }
  }

  cv::imshow("clone", clone);
  /*

  for (int i = 0; i < m_width - clusterWidth; i = i + i_shift) {     // TODO: change to i++
    for (int j = 0; j < m_height - clusterHeight; j = j + j_shift) { // TODO: change to j++
      cv::Rect rect(i, j, clusterWidth, clusterHeight);
      cv::Mat kernel = clone(rect);
      kernels.push_back(kernel.clone());

      cv::Mat VAL = cv::Mat(clusterHeight, clusterWidth, CV_16UC1, cv::Scalar(0));
      kernelsVAL.push_back(VAL.clone());
      // cv::Mat Out = cv::Mat(clusterHeight, clusterWidth, CV_8UC1, cv::Scalar(0));
      // ViterbiOut.push_back(VAL.clone());
    }
  }
  */
  Logger->critical("kernel start:");
  if (m_kernels.size() > m_range) {
    m_kernels.pop_front();
    m_kernels.push_back(kernels);
    m_kernelsVAL.pop_front();
    m_kernelsVAL.push_back(kernelsVAL);

  } else {
    m_kernels.push_back(kernels);
    m_kernelsVAL.push_back(kernelsVAL);
  }

  int m_kernelsSize = m_kernels.size();
  Logger->critical("Viterbi::m_kernelsVAL.size:{}", m_kernelsVAL.size());
  if (m_kernelsSize > 5 && m_kernelsVAL.size() > 5) {
    for (int z = 0; z < m_kernelsSize; z++) { // wielokosc kolejnki
      Logger->critical("z:{} add (m_kernels.size() - 1):{}", z, (m_kernelsSize - 1));
      Logger->critical("m_kernels[z].size():{}", m_kernels[z].size());
      for (int kernel = 0; kernel < m_kernels[z].size(); kernel++) { // ilosc kerneli
        //Logger->critical("kernel:{}", kernel);
        if (z == 0) {
          for (int i = 0; i < clusterWidth; i++) {
            for (int j = 0; j < clusterHeight; j++) {
              m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
                  unsigned short(m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)));
            }
          }
        } else {
          for (int i = m_treck; i < clusterWidth - m_treck; i++) {
            for (int j = m_treck; j < clusterHeight - m_treck; j++) {
              std::vector<unsigned char> temp;

              for (int ii = -m_treck; ii <= m_treck; ii++) {
                for (int jj = -m_treck; jj <= m_treck; jj++) {
                  temp.push_back(m_kernelsVAL[z - 1][kernel].at<unsigned short>(cv::Point(j + jj, i + ii)));
                }
              }

              std::vector<unsigned char>::iterator result;
              result = std::max_element(temp.begin(), temp.end());
              int distance = std::distance(temp.begin(), result);
              m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
                  unsigned short((m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)) + temp[distance]));
            }
          }
        }
        if (z == (m_kernelsSize - 1)) {
          double minVal;
          double maxVal;
          cv::Point minLoc;
          cv::Point maxLoc;
          //cv::Mat roi;
          cv::Rect rect(m_treck+1, m_treck+1, clusterWidth - 2 * m_treck-2, clusterHeight - 2 * m_treck-2);
          cv::Mat roi = m_kernelsVAL[z][kernel](rect);


          cv::minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc);

          //std::cout << "kernel:" << kernel << " min val: " << minVal << std::endl;
          //std::cout << "kernel:" << kernel << "max val: " << maxVal << " at pos:" << maxLoc << std::endl;

          cv::Mat Out = cv::Mat(clusterHeight, clusterWidth, CV_8UC1, cv::Scalar(0));
          Out.at<unsigned char>(maxLoc) = 255;

          ViterbiOut.push_back(Out);
          min.push_back(minLoc);
          max.push_back(maxLoc);
        }
      }
    }
  }
  Logger->critical("after (m_kernelsSize - 1):{}",(m_kernels.size() - 1));
#if (DEBUG)
  if (m_kernels.size() > 5 && m_kernelsVAL.size() > 5) {
    Logger->critical("debug start:");
    int iter = 0;
    for (int i = 0; i < m_width - clusterWidth; i = i + i_shift) {     // TODO: change to i++
      for (int j = 0; j < m_height - clusterHeight; j = j + j_shift) { // TODO: change to j++

        cv::Rect rect(i, j, clusterWidth, clusterHeight);
        cv::Mat kernelOut = ViterbiOutGlobal(rect);

        if (iter >= ViterbiOut.size()) {
          break;
        }
        if (kernelOut.cols == ViterbiOut[iter].cols) {
 
          cv::add(kernelOut, ViterbiOut[iter], kernelOut);

          kernelOut.copyTo(ViterbiOutGlobal(rect));
          //auto max[iter].x;
          //cv::Point point = cv::Point(max[iter].x + i, max[iter].y + j);
          //cv::drawMarker(preview, cv::Point(max[iter].x + i, max[iter].y + j), cv::Scalar(0, 0, 255), 0, 6, 1, 8);

          cv::Rect out(max[iter].x + i, max[iter].y + j, 6, 6);
          cv::rectangle(preview, out, cv::Scalar(0, 0,255), 0.8, 8);
          //cv::drawMarker(preview, cv::Point( min[iter].x + i, min[iter].y + j), cv::Scalar(255, 0, 255), 4, 6, 1, 8);
          //cv::rectangle(preview, rect, cv::Scalar(0, 0, 0), 0.8, 8);

        } else {

        }

        iter++;
        if (iter >= ViterbiOut.size()) {
          break;
        }
      }
    }

    
    cv::imshow("ViterbiOutGlobal", ViterbiOutGlobal);
    cv::imshow("preview", preview);
    cv::imwrite("preview.png",preview);

    for (int i = 0; i < ViterbiOut.size(); i++) {
     // QString nazwa = "ViterbiOut[" + QString::number(i) + "]";
      //cv::imshow(nazwa.toStdString(), ViterbiOut[i]);
    }
    
    std::vector<cv::Mat> m_images_CAT_vector;
    std::vector<cv::Mat> m_images_CAT;
    // cv::Mat m_images_CAT_thresh;
    for (int i = 0; i < m_kernelsVAL.size(); i++) {
      cv::Mat m_images_CAT_temp;
      cv::Mat m_images_temp;
      cv::Mat m_images_temp_color;
      if (i == m_kernelsVAL.size() - 1) {
        for (int kernel = 0; kernel < m_kernelsVAL[i].size(); kernel++) {
          QString nazwa = "kernel[" + QString::number(kernel) + "]";
          cv::imshow(nazwa.toStdString(), m_kernelsVAL[i][kernel]);
          /*
          m_kernelsVAL[i][kernel].convertTo(m_images_temp, CV_8UC1, 1, 0);
          cv::cvtColor(m_images_temp, m_images_temp_color, cv::COLOR_GRAY2BGR);
          if (kernel == 0) {
            cv::hconcat(m_images_temp_color, m_images_temp_color, m_images_CAT_temp);
          } else {
            cv::hconcat(m_images_CAT_temp, m_images_temp_color, m_images_CAT_temp);
          }*/
        }
      }
      // cv::normalize(m_images[i], m_images_temp, 0, 255, cv::NORM_MINMAX, CV_8UC1);
      m_images_CAT.push_back(m_images_CAT_temp);
    }
    Logger->critical("Viterbi::m_images_CAT.size:{}", m_images_CAT.size());
    for (int kernel = 0; kernel < m_images_CAT.size(); kernel++) {
      cv::resize(m_images_CAT[kernel], m_images_CAT[kernel], cv::Size(1000, 300), 0, 0, cv::INTER_NEAREST);
      QString nazwa = "m_images_CAT[" + QString::number(kernel) + "]";
      cv::imshow(nazwa.toStdString(), m_images_CAT[kernel]);
    }
    
    m_images_CAT.clear();
    m_images_CAT.shrink_to_fit();
    
    // cv::resize(m_images_CAT[kernel][i], m_images_CAT[kernel][i], cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
    // cv::imshow("VAL", m_images_CAT[kernel][i]);

    // cv::resize(m_images_CAT_thresh, m_images_CAT_thresh, cv::Size(2800, 300), 0, 0, cv::INTER_NEAREST);
    // cv::imshow("input_thresh", m_images_CAT_thresh);

    cv::waitKey(0);
  }
#endif
}
