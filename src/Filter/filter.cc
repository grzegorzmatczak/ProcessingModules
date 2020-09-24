#include "filter.h"
#include "filterlist.h"

Filter::Filter(QObject *parent) : Block(parent) {
  m_baseFilter = new Filters::None{};
}

void Filter::configure(QJsonObject const &a_config) {
  auto const NAME_STRING{a_config[NAME].toString()};
  spdlog::trace("filter type: {}", NAME_STRING.toStdString());
  delete m_baseFilter;
  m_timer.reset();

  if (NAME_STRING == "Color") {
    m_baseFilter = new Filters::Color{a_config};
  } else if (NAME_STRING == "Resize") {
    m_baseFilter = new Filters::Resize{a_config};
  } else if (NAME_STRING == "Threshold") {
    m_baseFilter = new Filters::Threshold{a_config};
  } else if (NAME_STRING == "MedianBlur") {
    m_baseFilter = new Filters::MedianBlur{a_config};
  } else if (NAME_STRING == "MorphologyOperation") {
    m_baseFilter = new Filters::MorphologyOperation{ a_config };
  } else if (NAME_STRING == "BilateralFilter") {
    m_baseFilter = new Filters::BilateralFilter{a_config};
  } else if (NAME_STRING == "Blur") {
    m_baseFilter = new Filters::Blur{a_config};
  } else if (NAME_STRING == "Sobel") {
    m_baseFilter = new Filters::Sobel{a_config};
  } else if (NAME_STRING == "Canny") {
    m_baseFilter = new Filters::Canny{a_config};
  } else if (NAME_STRING == "InRange") {
    m_baseFilter = new Filters::InRange{a_config};
  } else if (NAME_STRING == "BitwiseNot") {
    m_baseFilter = new Filters::BitwiseNot{ a_config };
  } else if (NAME_STRING == "RegionOfInterest") {
    m_baseFilter = new Filters::RegionOfInterest{ a_config };
  } else if (NAME_STRING == "GaussianBlur") {
    m_baseFilter = new Filters::GaussianBlur{ a_config };
  } else if (NAME_STRING == "AddGaussianNoise") {
    m_baseFilter = new Filters::AddGaussianNoise{ a_config };
  } else if (NAME_STRING == "AddDron") {
    m_baseFilter = new Filters::AddDron{ a_config };
  } else if (NAME_STRING == "FindContours") {
    m_baseFilter = new Filters::FindContours{ a_config };
  } else if (NAME_STRING == "None") {
    m_baseFilter = new Filters::None{};
  } else {
    //H_logger->error("Unsupported filter type: {}", NAME_STRING.toStdString());
  }
}

void Filter::process(std::vector<_data> &_data) {
  //H_logger->trace("Filter::process(a_image)");
  m_timer.start();

  if (_data[0].processing.empty()) {
    spdlog::error("Filter::process() image is empty!");
  } else {
    spdlog::trace("Filter::process() image is correct");
  }
  assert(_data[0].processing.empty() == false);
 // CV_ASSERT(_data[0].processing.empty() == false);

  m_baseFilter->process(_data);
  m_timer.stop();
}
double Filter::getElapsedTime() { return m_timer.getTimeMilli(); }
