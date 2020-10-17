#include "../include/processing.h"
#include "Adder/adder.h"
#include "Filter/filter.h"
#include "Subtractor/subtractor.h"

Processing::Processing(QObject *parent)
  : QObject(parent)
{
}

Processing *Processing::make(QString model)
{
  Logger->trace("Creating model: {}", model.toStdString());
  if (model == "Filter") {
    return new Filter(nullptr);
  } else if (model == "Estimator") {
    return new Subtractor(nullptr);
  } else if (model == "Adder") {
    return new Adder(nullptr);
  }
  return new Filter(nullptr);
}
