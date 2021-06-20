#include "custom_viterbi.h"
#include <QJsonObject>
#include <iostream>
#include <vector>

constexpr auto TRECK{ "Treck" };
constexpr auto RANGE{ "Range" };
constexpr auto CLUSTER_WIDTH{ "ClusterWidth" };
constexpr auto CLUSTER_HEIGHT{ "ClusterHeight" };
constexpr auto ABS_FILTER{ "AbsFilter" };
constexpr auto SHIFT_WIDTH{ "ShiftWidth" };
constexpr auto SHIFT_HEIGHT{ "ShiftHeight" };

#define DEBUG FALSE
#define TIMER FALSE

#if (TIMER)
cv::TickMeter m_timer;
#endif
#if (DEBUG)
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
#endif

Filters::Viterbi::Viterbi(QJsonObject const &a_config)
  : m_treck(a_config[TRECK].toInt())
  , m_range(a_config[RANGE].toInt())
  , clusterWidth(a_config[CLUSTER_WIDTH].toInt())
  , clusterHeight(a_config[CLUSTER_HEIGHT].toInt())
  , m_absFilter(a_config[ABS_FILTER].toBool())
  , i_shift(a_config[SHIFT_WIDTH].toInt())
  , j_shift(a_config[SHIFT_HEIGHT].toInt())
{
  m_firstTime = true;
  m_width = 0;
  m_height = 0;

#if (TIMER)
  Logger->debug("m_treck:{}", m_treck);
  Logger->debug("m_range:{}", m_range);
  Logger->debug("clusterWidth:{}", clusterWidth);
  Logger->debug("clusterHeight:{}", clusterHeight);
  Logger->debug("m_absFilter:{}", m_absFilter);
  Logger->debug("i_shift:{}", i_shift);
  Logger->debug("j_shift:{}", j_shift);
#endif
}


void Filters::Viterbi::process(std::vector<_data> &_data)
{
#if (TIMER)
  m_timer.start();
#endif
  int m_treck = 1;
  int m_range = 10;
  int clusterWidth = 25;
  int clusterHeight = 25;
  bool m_absFilter = true;

  if (m_firstTime) {
    m_firstTime = false;
#if (DEBUG)
    cv::imshow("input", _data[0].processing);
    Logger->trace("Viterbi::process() size:{}x{}", _data[0].processing.cols, _data[0].processing.rows);
#endif
    m_width = _data[0].processing.cols;
    m_height = _data[0].processing.rows;
  }

  cv::Mat clone = _data[0].processing.clone();

  cv::Mat preview;
  cv::cvtColor(_data[0].processing, preview, cv::COLOR_GRAY2BGR);

  Logger->trace("push to deque:");
  std::vector<cv::Mat> kernels;
  std::vector<cv::Mat> kernelsVAL;
  std::vector<cv::Mat> ViterbiOut;
  std::vector<cv::Point> min;
  std::vector<cv::Point> max;
  std::vector<cv::Point> means;

  

  for (int i = 0; i <= m_width - clusterWidth; i = i + i_shift) {
    for (int j = 0; j <= m_height - clusterHeight; j = j + j_shift) {
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
  std::vector<unsigned char> temp;
  temp.reserve(m_treck * 2 + 1);
#if (TIMER)
  m_timer.stop();
  double time = m_timer.getTimeMilli();
  Logger->debug("Viterbi Init:{:f}", time);
  m_timer.reset();
 #endif

  if (m_kernelsSize > 5 && m_kernelsVAL.size() > 5) {
    for (int z = 0; z < m_kernelsSize; z++) { // wielokosc kolejnki
#if (TIMER)
      m_timer.start();
#endif
      for (int kernel = 0; kernel < m_kernels[z].size(); kernel++) { // ilosc kerneli
        if (z == 0) {
          for (int i = 0; i < clusterWidth; i++) {
            for (int j = 0; j < clusterHeight; j++) {
              m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
                  (unsigned short)(m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)));
            }
          }
        } else {
          for (int i = m_treck; i < clusterWidth - m_treck; i++) {
            for (int j = m_treck; j < clusterHeight - m_treck; j++) {
              temp.clear();

              for (int ii = -m_treck; ii <= m_treck; ii++) {
                for (int jj = -m_treck; jj <= m_treck; jj++) {
                  temp.push_back(m_kernelsVAL[z - 1][kernel].at<unsigned short>(cv::Point(j + jj, i + ii)));
                }
              }

              std::vector<unsigned char>::iterator result;
              result = std::max_element(temp.begin(), temp.end());
              int distance = std::distance(temp.begin(), result);
              m_kernelsVAL[z][kernel].at<unsigned short>(cv::Point(j, i)) =
                 ( unsigned short)((m_kernels[z][kernel].at<unsigned char>(cv::Point(j, i)) + temp[distance]));
            }
          }
        }
        if (z == (m_kernelsSize - 1)) {
          double minVal;
          double maxVal;
          cv::Point minLoc;
          cv::Point maxLoc;
          cv::Rect rect(0, 0, clusterWidth, clusterHeight);
          cv::Mat roi = m_kernelsVAL[z][kernel](rect);
          cv::Mat roiThresh;

          cv::minMaxLoc(roi, &minVal, &maxVal, &minLoc, &maxLoc);
          double tresh = maxVal * 0.98;
          cv::threshold(roi, roiThresh, int(tresh), 255, 0);
          roiThresh.convertTo(roiThresh, CV_8UC1);
          
          ViterbiOut.push_back(roiThresh);
          min.push_back(minLoc);
          max.push_back(maxLoc);
#if (DEBUG)
          std::string type = type2str(roiThresh.type());

          int histSize = 256;
          float range[] = { 0, 256 };
          const float *histRange = { range };
          bool uniform = true;
          bool accumulate = false;
          cv::Mat grey_hist;
          // Compute the histograms:
          cv::calcHist(&m_kernelsVAL[z][kernel], 1, 0, cv::Mat(), grey_hist, 1, &histSize, &histRange, uniform,
                       accumulate);
          // Draw the histograms
          int hist_w = 512;
          int hist_h = 400;
          int bin_w = cvRound((double)hist_w / histSize);
          cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
          // Normalize the result to [ 0, histImage.rows ]
          normalize(grey_hist, grey_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
          // Draw for each channel
          for (int i = 1; i < histSize; i++) {
            line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(grey_hist.at<float>(i - 1))),
                 cv::Point(bin_w * (i), hist_h - cvRound(grey_hist.at<float>(i))), cv::Scalar(120, 120, 120), 1, 8, 0);
          }
          imshow("Hist", histImage);
#endif
        }
      }
#if (TIMER)
      m_timer.stop();
      time = m_timer.getTimeMilli();
      Logger->debug("Viterbi range:{}:    {:f}", z, time);
      m_timer.reset();
#endif
    }
  }

  cv::Mat ViterbiOutGlobal = cv::Mat(m_height, m_width, CV_8UC1, cv::Scalar(0));

  if (m_kernels.size() > 5 && m_kernelsVAL.size() > 5) {
    Logger->trace("debug start:");
    int iter = 0;
    for (int i = 0; i <= m_width - clusterWidth; i = i + i_shift) {
      for (int j = 0; j <= m_height - clusterHeight; j = j + j_shift) {

        cv::Rect rect(i, j, clusterWidth, clusterHeight);
        cv::Mat kernelOut = ViterbiOutGlobal(rect);

        if (iter >= ViterbiOut.size()) {
          break;
        }
        if (kernelOut.cols == ViterbiOut[iter].cols) {
          cv::add(kernelOut, ViterbiOut[iter], kernelOut);

          kernelOut.copyTo(ViterbiOutGlobal(rect));

          cv::Rect out(max[iter].x + i, max[iter].y + j, 6, 6);
          cv::rectangle(preview, out, cv::Scalar(0, 0, 255), 0.8, 8);
        }
        iter++;
        if (iter >= ViterbiOut.size()) {
          break;
        }
      }
    }
  }
#if (DEBUG)
  cv::waitKey(0);
#endif
#if (TIMER)
  m_timer.stop();
#endif
  // 0:
  _data[0].processing = preview.clone();
  // 1:
  struct _data data2;
  data2.processing = ViterbiOutGlobal.clone();
  _data.push_back(data2);

  
}
