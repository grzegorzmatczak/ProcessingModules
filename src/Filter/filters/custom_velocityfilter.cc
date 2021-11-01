#include "custom_velocityfilter.h"

#define DEBUG_OPENCV

Filters::VelocityFilter::VelocityFilter(QJsonObject const &a_config)
{
	Logger->debug("Filters::VelocityFilter::VelocityFilter(...)");
	m_viterbi = new viterbi::VelocityFilter_impl(a_config);
}

Filters::VelocityFilter::~VelocityFilter()
{
	Logger->debug("Filters::VelocityFilter::~VelocityFilter()");
	delete m_viterbi;
}

void Filters::VelocityFilter::process(std::vector<_data> &_data)
{
	
	m_viterbi->forwardStep(_data[0].processing);
	//m_viterbi->backwardStep();
	_data[0].processing = m_viterbi->getOutput();
	/*
	struct _data data2;
	data2.processing = ViterbiOutGlobal;
	_data.push_back(data2);
	_data[0].processing */
}
