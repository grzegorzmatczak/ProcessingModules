#include "custom_viterbifilter.h"

#include <QJsonObject>

#define DEBUG

Filters::ViterbiFilter::ViterbiFilter(QJsonObject const &a_config)
{
	#ifdef DEBUG
	//Logger->debug("{}() {}:", __FUNCTION__, __LINE__);
	#endif
	m_viterbi = new viterbi::ViterbiFilter_impl(a_config);
}

Filters::ViterbiFilter::~ViterbiFilter()
{
	#ifdef DEBUG
	//Logger->debug("{}() {}:", __FUNCTION__, __LINE__);
	#endif
	delete m_viterbi;
}

void Filters::ViterbiFilter::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	//Logger->debug("{}() _data.size:{}", __FUNCTION__, _data.size());
	#endif
	m_viterbi->nextIteration(_data);
	m_viterbi->forwardStep();
	m_viterbi->backwardStep();
	_data[0].processing = m_viterbi->getOutput();
}
