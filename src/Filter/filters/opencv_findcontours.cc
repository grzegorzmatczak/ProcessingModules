#include "opencv_findcontours.h"

#include <QJsonObject>
#include <QJsonArray>

#include <opencv2/imgproc.hpp>

constexpr auto DRAW_CONTOURS_VALID{ "DrawContoursValid" };
constexpr auto DRAW_CONTOURS_NOT_VALID{ "DrawContoursNotValid" };
constexpr auto MIN_CONTOUR_SIZE{ "MinContourSize" };
constexpr auto MAX_CONTOUR_SIZE{ "MaxContourSize" };
constexpr auto SAVE_CONTOURS{ "SaveContours" };

Filters::FindContours::FindContours(QJsonObject const &a_config)
  : m_drawContoursValid{ a_config[DRAW_CONTOURS_VALID].toBool() }
  , m_drawContoursNotValid{ a_config[DRAW_CONTOURS_NOT_VALID].toBool() }
  , m_minContourSize{ a_config[MIN_CONTOUR_SIZE].toDouble() }
  , m_maxContourSize{ a_config[MAX_CONTOUR_SIZE].toDouble() }
  , m_saveContours{ a_config[SAVE_CONTOURS].toBool() }
{
}

void Filters::FindContours::process(std::vector<_data> &_data)
{
  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(_data[0].processing, contours, hierarchy, cv::RETR_EXTERNAL, cv::RETR_LIST, cv::Point(0, 0));
  std::vector<std::vector<cv::Point>> contoursBEST;
  std::vector<cv::Vec4i> hierarchyBEST;
  std::vector<cv::Point> approx;
  double area;

  if (m_drawContoursValid || m_drawContoursNotValid) {
    if (_data.size() <= 1) {
      struct _data data;
      data.processing = _data[0].processing.clone();
      _data.push_back(data);
      m_colorValid = cv::Scalar(100);
      m_colorNotValid = cv::Scalar(200);
    } else {
      m_colorValid = cv::Scalar(0, 255, 0);    // green
      m_colorNotValid = cv::Scalar(0, 0, 255); // red
    }
  }

  for (unsigned int i = 0; i < contours.size(); i++) {
    area = cv::contourArea(contours[i]);
    if ((area >= m_minContourSize) && (area <= m_maxContourSize)) {
      contoursBEST.push_back(contours[i]);
      hierarchyBEST.push_back(hierarchy[i]);
      if (m_drawContoursValid) {
        cv::drawContours(_data[1].processing, contours, static_cast<int>(i), m_colorValid, 1, 1);
      }

    } else {
      if (m_drawContoursNotValid) {
        cv::drawContours(_data[1].processing, contours, static_cast<int>(i), m_colorNotValid, 1, 1);
      }
    }
  }

  if (m_saveContours) {
    QJsonArray ROIArray;

    for (unsigned int i = 0; i < contoursBEST.size(); i++) {
      cv::approxPolyDP(cv::Mat(contoursBEST[i]), approx, cv::arcLength(cv::Mat(contoursBEST[i]), true) * 0.04, true);
      area = cv::contourArea(contoursBEST[i]);
      cv::Rect boundRect;
      boundRect = cv::boundingRect(contoursBEST[i]);

      //_data[0].rects.push_back(boundRect);
    }
  }

}
