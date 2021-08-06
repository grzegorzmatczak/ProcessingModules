#include "filter.h"
#include "filterlist.h"

//#define DEBUG_FILTERS_MODULES

constexpr auto NAME{ "Name" };

Filter::Filter(QObject *parent)
	: Processing(parent)
{
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::Filter()");
	#endif

	m_baseFilter = new Filters::None{};
}

void Filter::configure(QJsonObject const &a_config)
{
	auto const _name{ a_config[NAME].toString() };
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::configure() filter type: {}", _name.toStdString());
	#endif
	delete m_baseFilter;
	m_timer.reset();

	if (_name == "Color") {
	m_baseFilter = new Filters::Color{ a_config };
	} else if (_name == "Resize") {
	m_baseFilter = new Filters::Resize{ a_config };
	} else if (_name == "Threshold") {
	m_baseFilter = new Filters::Threshold{ a_config };
	} else if (_name == "MedianBlur") {
	m_baseFilter = new Filters::MedianBlur{ a_config };
	} else if (_name == "MorphologyOperation") {
	m_baseFilter = new Filters::MorphologyOperation{ a_config };
	} else if (_name == "BilateralFilter") {
	m_baseFilter = new Filters::BilateralFilter{ a_config };
	} else if (_name == "Blur") {
	m_baseFilter = new Filters::Blur{ a_config };
	} else if (_name == "Sobel") {
	m_baseFilter = new Filters::Sobel{ a_config };
	} else if (_name == "Canny") {
	m_baseFilter = new Filters::Canny{ a_config };
	} else if (_name == "InRange") {
	m_baseFilter = new Filters::InRange{ a_config };
	} else if (_name == "BitwiseNot") {
	m_baseFilter = new Filters::BitwiseNot{ a_config };
	} else if (_name == "RegionOfInterest") {
	m_baseFilter = new Filters::RegionOfInterest{ a_config };
	} else if (_name == "GaussianBlur") {
	m_baseFilter = new Filters::GaussianBlur{ a_config };
	} else if (_name == "AddGaussianNoise") {
	m_baseFilter = new Filters::AddGaussianNoise{ a_config };
	} else if (_name == "AddMultipleDron") {
	m_baseFilter = new Filters::AddMultipleDron{ a_config };
	} else if (_name == "FindContours") {
	m_baseFilter = new Filters::FindContours{ a_config };
	} else if (_name == "Viterbi") {
	m_baseFilter = new Filters::Viterbi{ a_config };
	} else if (_name == "VelocityFilter") {
	m_baseFilter = new Filters::VelocityFilter{ a_config };
	} else if (_name == "None") {
	m_baseFilter = new Filters::None{};
	} else {
	Logger->error("Filter::configure() Unsupported filter type: {}", _name.toStdString());
	}
}

void Filter::process(std::vector<_data> &_data)
{
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::process()");
	#endif
	m_timer.start();
	m_baseFilter->process(_data);
	m_timer.stop();
}
double Filter::getElapsedTime()
{
	return m_timer.getTimeMilli();
}
