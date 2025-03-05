#include "custom_velocityfilter.h"

#include <QJsonObject>

//#define DEBUG

Filters::VelocityFilter::VelocityFilter(QJsonObject const &a_config)
{
	#ifdef DEBUG
	Logger->debug("VelocityFilter::VelocityFilter()");
	#endif
	m_viterbi = new viterbi::VelocityFilter_impl(a_config);
}

Filters::VelocityFilter::~VelocityFilter()
{
	#ifdef DEBUG
	Logger->debug("VelocityFilter::~VelocityFilter()");
	#endif
	delete m_viterbi;
}

void Filters::VelocityFilter::process(std::vector<_data> &_data)
{
	
	m_viterbi->forwardStep();
	//m_viterbi->backwardStep();
	_data[0].processing = m_viterbi->getOutput();
	/*
	struct _data data2;
	data2.processing = ViterbiOutGlobal;
	_data.push_back(data2);
	_data[0].processing */
}
