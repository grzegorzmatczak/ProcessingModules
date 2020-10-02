#include "../include/block.h"
#include "Adder/adder.h"
#include "Filter/filter.h"
#include "Subtractor/subtractor.h"

Block::Block(QObject *parent) : QObject(parent) {

spdlog::set_level(static_cast<spdlog::level::level_enum>(0));
  spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
}

Block *Block::make(QString model) {
  //qInfo() << "Creating: " << model;
  //spdlog->trace("Filter type: {}", model.toStdString());
  qInfo() << "Creating: " << model;
  if (model == "Filter") {
    return new Filter(nullptr);
  } else if (model == "Estimator") {
    return new Subtractor(nullptr);
  } else if (model == "Adder") {
    return new Adder(nullptr);
  }
  return new Filter(nullptr);
}
