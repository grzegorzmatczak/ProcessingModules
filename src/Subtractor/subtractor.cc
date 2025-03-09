#include "subtractor.h"
#include "subtractorlist.h"

#include <QJsonObject>
#include <QString>
//#define DEBUG



Subtractor::Subtractor(QObject *parent)
	: Processing(parent)
{
	#ifdef DEBUG
	//Logger->debug("Subtractor::Subtractor()");
	#endif
	m_subtractor = new Subtractors::None();
}

void Subtractor::configure(QJsonObject const &a_config)
{
	#ifdef DEBUG
	//Logger->debug("Filter::Filter()");
	#endif
	delete m_subtractor;
	m_timer.reset();
	auto const _name{ a_config[NAME].toString() };
	#ifdef DEBUG
	H_logger->debug("Subtractor type: {}", _name.toStdString());
	#endif
	if (_name == "GSOC")
	{
		m_subtractor = { new Subtractors::GSOC{ a_config } };
	}
	else if (_name == "KNN")
	{
		m_subtractor = { new Subtractors::KNN{ a_config } };
	}
	else if (_name == "MOG")
	{
		m_subtractor = { new Subtractors::MOG{ a_config } };
	}
	else if (_name == "MOG2")
	{
		m_subtractor = { new Subtractors::MOG2{ a_config } };
	}
	else if (_name == "GMG")
	{
		m_subtractor = { new Subtractors::GMG{ a_config } };
	}
	else if (_name == "CNT")
	{
		m_subtractor = { new Subtractors::CNT{ a_config } };
	}
	else if (_name == "LSBP")
	{
		m_subtractor = { new Subtractors::LSBP{ a_config } };
	}
	else if (_name == "NONE")
	{
		m_subtractor = { new Subtractors::None{} };
	}
	else if (_name == "MEDIAN")
	{
		m_subtractor = { new Subtractors::Median{ a_config } };
	}
	else if (_name == "MEDIAN2")
	{
		m_subtractor = { new Subtractors::Median2{ a_config } };
	}
	else if (_name == "ViBe")
	{
		m_subtractor = { new Subtractors::ViBe{ a_config } };
	}
	else if (_name == "ABL")
	{
		m_subtractor = { new Subtractors::ABL{ a_config } };
	}
	else if (_name == "ASBL")
	{
		m_subtractor = { new Subtractors::ASBL{ a_config } };
	}
	else if (_name == "LOBSTER")
	{
		m_subtractor = { new Subtractors::LOBSTER{ a_config } };
	}
	else if (_name == "WMV")
	{
		m_subtractor = { new Subtractors::WeightedMovingVariance{ a_config } };
	}
	else if (_name == "WMM")
	{
		m_subtractor = { new Subtractors::WeightedMovingMean{ a_config } };
	}
	else if (_name == "PBAS")
	{
		m_subtractor = { new Subtractors::PixelBasedAdaptiveSegmenter{ a_config } };
	}
	else
	{
		//Logger->error("Unsupported subtractor type:{}", _name.toStdString());
	}
}

void Subtractor::process(std::vector<_data> &_data)
{
	m_timer.start();

	if (_data[0].processing.empty())
	{
		//Logger->error("Subtractor::process() image is empty!");
	}

	m_subtractor->process(_data);
	m_timer.stop();
}

double Subtractor::getElapsedTime()
{
	return m_timer.getTimeMilli();
}
