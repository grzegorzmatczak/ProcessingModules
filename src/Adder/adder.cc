#include "adder.h"
#include "adderlist.h"

constexpr auto NAME{ "Name" };

Adder::Adder(QObject *parent) : Block(parent) { m_adder = new Adders::None();
  spdlog::set_level(static_cast<spdlog::level::level_enum>(0));
  spdlog::set_pattern("[%Y-%m-%d] [%H:%M:%S.%e] [%t] [%^%l%$] %v");
}

void Adder::configure(QJsonObject const &a_config) {
  // H_logger->trace("Adder::configure()");
  delete m_adder;
  m_timer.reset();
  auto const NAME_STRING{a_config[NAME].toString()};
  //spdlog->trace("Adder type: {}", NAME_STRING.toStdString());

  if (NAME_STRING == "Add") {
    m_adder = {new Adders::Add{a_config}};
  } else if (NAME_STRING == "AbsDiff") {
    m_adder = { new Adders::AbsDiff{} };
  } else if (NAME_STRING == "AddWeighted") {
    m_adder = {new Adders::AddWeighted{a_config}};
  } else if (NAME_STRING == "None") {
    m_adder = {new Adders::None{}};
  } else {
    // H_logger->error("Unsupported Adder type: {}", NAME_STRING.toStdString());
  }
}

void Adder::process(std::vector<_data> &_data) {
  m_timer.start();
  m_adder->process(_data);
  m_timer.stop();
}

double Adder::getElapsedTime() { return m_timer.getTimeMilli(); }
