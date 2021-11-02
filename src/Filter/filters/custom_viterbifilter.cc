#include "custom_viterbifilter.h"

//#define DEBUG

Filters::ViterbiFilter::ViterbiFilter(QJsonObject const &a_config)
{
	#ifdef DEBUG
	Logger->debug("ViterbiFilter::ViterbiFilter()");
	#endif
	m_viterbi = new viterbi::ViterbiFilter_impl(a_config);
}

Filters::ViterbiFilter::~ViterbiFilter()
{
	#ifdef DEBUG
	Logger->debug("ViterbiFilter::~ViterbiFilter()");
	#endif
	delete m_viterbi;
}

void Filters::ViterbiFilter::process(std::vector<_data> &_data)
{
	
	m_viterbi->forwardStep(_data[0].processing);
	m_viterbi->backwardStep();
	_data[0].processing = m_viterbi->getOutput();
	/*
	struct _data data2;
	data2.processing = ViterbiOutGlobal;
	_data.push_back(data2);
	_data[0].processing */
}
