#include <QApplication>
#include <QCoreApplication>
#include <QJsonObject>
#include <QMetaEnum>

#include "structures.h"
#include "block.h"
#include "mainloop.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define floatqDebug() qDebug() << fixed << qSetRealNumberPrecision(2)

int main(int arg, char *argv[])
{
  H_Logger->set_level(static_cast<spdlog::level::level_enum>(0));
  H_Logger->set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
  H_Logger->debug("start main logger with LogLevel:{}", 0);

  QApplication a(arg, argv);

  MainLoop mainLoop{};
  QObject::connect(&mainLoop, &MainLoop::quit, &a, &QApplication::quit);

  return a.exec();
}
