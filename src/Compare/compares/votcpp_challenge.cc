#include "votcpp_challenge.h"
#include <QFile>
#include <QLoggingCategory>
#include <QPolygonF>
Q_LOGGING_CATEGORY(CompareVotCppLogger, "Compare.votcpp")

Compares::VOTCpp::VOTCpp(QJsonObject const &a_config) : m_counter(0) {
  QFile _textFile{a_config[GROUNDTRUTH].toString()};
  if (_textFile.open(QIODevice::ReadOnly)) {
    while (!_textFile.atEnd()) {
      QString _line = _textFile.readLine();
      QStringList _listSplit = _line.split(',');
      QPolygonF polygon;
      QPointF(0, 0);
      for (int i = 0; i < 8; i++) {
        polygon << QPointF(0, 0);
      }

      // qCDebug(CompareVotCppLogger) << "_listSplit.size():" <<
      // _listSplit.size();
      if (_listSplit.size() == 4) {
      } else if (_listSplit.size() == 8) {
        polygon.clear();
        polygon << QPointF(_listSplit[0].toDouble(), _listSplit[1].toDouble())
                << QPointF(_listSplit[2].toDouble(), _listSplit[3].toDouble())
                << QPointF(_listSplit[4].toDouble(), _listSplit[5].toDouble())
                << QPointF(_listSplit[6].toDouble(), _listSplit[7].toDouble());
      }

      m_rect.push_back(polygon);
    }
  }

  for (int i = 0; i < m_rect.size(); i++) {
    if (i == 0) {
      QRectF data = m_rect[i].boundingRect();
      qCDebug(CompareVotCppLogger)
          << "data:" << data.x() << " " << data.y() << " " << data.width()
          << " " << data.height() << "\n";
      // qDebug() << "data:" << data.x() << " " << data.y() << " " <<
      // data.width()
      //         << " " << data.height() << "\n";
    }
  }
}

void Compares::VOTCpp::process(std::vector<_data> &_data) {

  qCDebug(CompareVotCppLogger) << "m_rect.size:" << m_rect.size();
  qCDebug(CompareVotCppLogger) << "m_counter:" << m_counter;
  qCDebug(CompareVotCppLogger)
      << "m_rect[m_counter].size():" << m_rect[m_counter].size();
  for (int i = 0; i < 3; i++) {
    cv::line(
        _data[0].processing,
        cv::Point2f(m_rect[m_counter].at(i).x(), m_rect[m_counter].at(i).y()),
        cv::Point2f(m_rect[m_counter].at(i + 1).x(),
                    m_rect[m_counter].at(i + 1).y()),
        cv::Scalar(255, 0, 0), 1);
  }
  cv::line(
      _data[0].processing,
      cv::Point2f(m_rect[m_counter].at(0).x(), m_rect[m_counter].at(0).y()),
      cv::Point2f(m_rect[m_counter].at(3).x(), m_rect[m_counter].at(3).y()),
      cv::Scalar(255, 0, 0), 1);

  for (int i = 0; i < _data[0].bounds.size(); i++) {
    if (m_rect[m_counter].intersects(_data[0].bounds[i])) {
      QPolygonF intersected = m_rect[m_counter].intersected(_data[0].bounds[i]);
      QPolygonF unia = m_rect[m_counter].united(_data[0].bounds[i]);
      // qDebug() << "m_rect[m_counter]:" << m_rect[m_counter];
      // qDebug() << "_data[0].bounds[0]:" << _data[0].bounds[i];
      // qDebug() << "intersected:" << intersected;
      // qDebug() << "unia:" << unia;

      std::vector<float> xPts;
      std::vector<float> yPts;
      for (int i = 0; i < intersected.size(); i++) {
        xPts.push_back(intersected.at(i).x());
        yPts.push_back(intersected.at(i).y());
      }
      // qDebug() << "xPts:" << xPts;
      // qDebug() << "yPts:" << yPts;

      int j = xPts.size() - 1;
      double area{0};
      for (int i = 0; i < xPts.size(); i++) {
        area += (xPts[j] + xPts[i] * (yPts[j] - yPts[i]));
        j = i;
      }
      area = area / 2.0;
      // qDebug() << "area:" << area;
      QString data = "tracker[" + QString::number(i) +
                     "]:" + QString::number(area, 'g', 6);
      cv::Point point = cv::Point(5, 20 + i * 30);
      cv::putText(_data[0].processing, data.toStdString(), point,
                  cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 0, 0), 3);
      cv::putText(_data[0].processing, data.toStdString(), point,
                  cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255), 1);

      for (int i = 0; i < intersected.size() - 1; i++) {
        cv::line(
            _data[0].processing,
            cv::Point2f(intersected.at(i).x(), intersected.at(i).y()),
            cv::Point2f(intersected.at(i + 1).x(), intersected.at(i + 1).y()),
            cv::Scalar(0, 0, 255), 1);
      }
      cv::line(_data[0].processing,
               cv::Point2f(intersected.at(0).x(), intersected.at(0).y()),
               cv::Point2f(intersected.at(intersected.size() - 1).x(),
                           intersected.at(intersected.size() - 1).y()),
               cv::Scalar(0, 0, 255), 1);
    }
  }
  /*
  //qDebug() << "compare:";

  //qDebug() << "_data[0].bounds.x" << _data[0].bounds[i].x;
  //qDebug() << "m_rect[" << m_counter << "].x:" << m_rect[m_counter].x;

  cv::Rect2d overlap = _data[0].bounds[i] & m_rect[m_counter];
  cv::Rect2d area = _data[0].bounds[i] | m_rect[m_counter];
  bool isOverlap = overlap.area() > 0;
  //qDebug() << "intersection.x" << overlap.x;
  //qDebug() << "area.x" << area.x;

  double unia =
      _data[0].bounds[i].area() + m_rect[m_counter].area() - overlap.area();

  double IoU = overlap.area() / unia;

  double overlapOnly1 = _data[0].bounds[i].area() / unia;
  double overlapOnly2 = m_rect[m_counter].area() / unia;
  double overlapAll = overlap.area();

  cv::rectangle(_data[0].processing, overlap, cv::Scalar(0, 255, 0), 2);
  cv::Point point = cv::Point(10, 50 + i * 30);
  QString data =
      "tracker[" + QString::number(i) + "]:" + QString::number(overlapAll);
  cv::putText(_data[0].processing, data.toStdString(), point,
              cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 0, 0), 3);
  cv::putText(_data[0].processing, data.toStdString(), point,
              cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255), 1);*/

  m_counter++;
}
