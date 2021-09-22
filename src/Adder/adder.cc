#include "adder.h"
#include "adderlist.h"

#define DEBUG_ADDER_MODULES

constexpr auto NAME{ "Name" };

Adder::Adder(QObject *parent) : Processing(parent) { m_adder = new Adders::None();
	#ifdef DEBUG_ADDER_MODULES
	Logger->debug("Adder::Adder()");
	#endif
}

void Adder::configure(QJsonObject const &a_config) {
	delete m_adder;
	m_timer.reset();
	auto const _name{ a_config[NAME].toString() };
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::configure() filter type: {}", _name.toStdString());
	#endif

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
		Logger->error("Adder::configure() Unsupported Adder type: {}", _name.toStdString());
	}
}

void Adder::process(std::vector<_data> &_data)
{
	m_timer.start();
	m_adder->process(_data);
	m_timer.stop();
}

double Adder::getElapsedTime() { return m_timer.getTimeMilli(); }
