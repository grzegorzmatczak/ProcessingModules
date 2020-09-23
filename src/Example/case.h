#ifndef CASE_H
#define CASE_H

#include <QObject>

#include "block.h"
#include "structures.h"

class Case : public QObject
{
  Q_OBJECT
 public:
  explicit Case(QJsonObject const &a_config, QObject *parent = nullptr);

 signals:
  void quit();

 private:
  cv::TickMeter m_timer;
};

#endif // CASE_H
