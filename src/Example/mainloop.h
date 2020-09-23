#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <QDebug>
#include <QJsonObject>
#include <QObject>
#include <QThread>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "structures.h"
#include "block.h"
#include "case.h"

class MainLoop : public QObject
{
  Q_OBJECT
 public:
  explicit MainLoop();

 signals:
  void quit();

 public slots:
  void onUpdate();
  void onQuit();
};

#endif // MAINLOOP_H
