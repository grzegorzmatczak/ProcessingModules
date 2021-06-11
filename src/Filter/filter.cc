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
	auto const NAME_STRING{ a_config[NAME].toString() };
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::configure() filter type: {}", NAME_STRING.toStdString());
	#endif
	delete m_baseFilter;
	m_timer.reset();

	if (NAME_STRING == "Color") {
	m_baseFilter = new Filters::Color{ a_config };
	} else if (NAME_STRING == "Resize") {
	m_baseFilter = new Filters::Resize{ a_config };
	} else if (NAME_STRING == "Threshold") {
	m_baseFilter = new Filters::Threshold{ a_config };
	} else if (NAME_STRING == "MedianBlur") {
	m_baseFilter = new Filters::MedianBlur{ a_config };
	} else if (NAME_STRING == "MorphologyOperation") {
	m_baseFilter = new Filters::MorphologyOperation{ a_config };
	} else if (NAME_STRING == "BilateralFilter") {
	m_baseFilter = new Filters::BilateralFilter{ a_config };
	} else if (NAME_STRING == "Blur") {
	m_baseFilter = new Filters::Blur{ a_config };
	} else if (NAME_STRING == "Sobel") {
	m_baseFilter = new Filters::Sobel{ a_config };
	} else if (NAME_STRING == "Canny") {
	m_baseFilter = new Filters::Canny{ a_config };
	} else if (NAME_STRING == "InRange") {
	m_baseFilter = new Filters::InRange{ a_config };
	} else if (NAME_STRING == "BitwiseNot") {
	m_baseFilter = new Filters::BitwiseNot{ a_config };
	} else if (NAME_STRING == "RegionOfInterest") {
	m_baseFilter = new Filters::RegionOfInterest{ a_config };
	} else if (NAME_STRING == "GaussianBlur") {
	m_baseFilter = new Filters::GaussianBlur{ a_config };
	} else if (NAME_STRING == "AddGaussianNoise") {
	m_baseFilter = new Filters::AddGaussianNoise{ a_config };
	} else if (NAME_STRING == "AddMultipleDron") {
	m_baseFilter = new Filters::AddMultipleDron{ a_config };
	} else if (NAME_STRING == "AddMultipleDronBlurred") {
	m_baseFilter = new Filters::AddMultipleDronBlurred{ a_config };
	} else if (NAME_STRING == "FindContours") {
	m_baseFilter = new Filters::FindContours{ a_config };
	} else if (NAME_STRING == "Viterbi") {
	m_baseFilter = new Filters::Viterbi{ a_config };
	} else if (NAME_STRING == "VelocityFilter") {
	m_baseFilter = new Filters::VelocityFilter{ a_config };
	} else if (NAME_STRING == "None") {
	m_baseFilter = new Filters::None{};
	} else {
	Logger->error("Filter::configure() Unsupported filter type: {}", NAME_STRING.toStdString());
	}
}

void Filter::process(std::vector<_data> &_data)
{
	#ifdef DEBUG_FILTERS_MODULES
	Logger->debug("Filter::process()");
	if (_data[0].processing.empty())
	{
		Logger->error("Filter::process() image is empty!");
	} 
	#endif
	m_timer.start();
	m_baseFilter->process(_data);
	m_timer.stop();
}
double Filter::getElapsedTime()
{
	return m_timer.getTimeMilli();
}
