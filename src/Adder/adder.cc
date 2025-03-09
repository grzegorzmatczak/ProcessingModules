#include "adder.h"
#include "adderlist.h"

#define DEBUG_ADDER_MODULES

#include "logger.hpp"

#include <QJsonObject>
#include <QJsonArray>
#include <QString>

Adder::Adder(QObject *parent) : Processing(parent) 
{ 
	m_adder = new Adders::None();
}

void Adder::configure(QJsonObject const &a_config) {
	delete m_adder;
	m_timer.reset();
	auto const _name{ a_config[NAME].toString() };

	if (_name == "Add")
	{
		m_adder = {new Adders::Add{a_config}};
	}
	else if (_name == "AbsDiff")
	{
		m_adder = { new Adders::AbsDiff{} };
	}
	else if (_name == "AddWeighted")
	{
		m_adder = {new Adders::AddWeighted{a_config}};
	}
	else if (_name == "None")
	{
		m_adder = {new Adders::None{}};
	}
	else
	{
		mLogger->print(QString("Unsupported Adder type:%1").arg(_name), logger::LogLevel::MEDIUM, __FUNCTION__);
	}
}

void Adder::process(std::vector<_data> &_data)
{
	m_timer.start();
	m_adder->process(_data);
	m_timer.stop();
}

double Adder::getElapsedTime() { return m_timer.getTimeMilli(); }
