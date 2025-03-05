#include "custom_viterbiglobal.h"

#include <QJsonObject>

#define DEBUG

Filters::ViterbiGlobal::ViterbiGlobal(QJsonObject const &a_config)
{
	#ifdef DEBUG
	//Logger->debug("{}() {}:", __FUNCTION__, __LINE__);
	#endif
	m_viterbi = new viterbi::ViterbiGlobal_impl(a_config);
}

Filters::ViterbiGlobal::~ViterbiGlobal()
{
	#ifdef DEBUG
	//Logger->debug("{}() {}:", __FUNCTION__, __LINE__);
	#endif
	delete m_viterbi;
}

void Filters::ViterbiGlobal::process(std::vector<_data> &_data)
{
	#ifdef DEBUG
	//Logger->debug("{}() _data.size:{}", __FUNCTION__, _data.size());
	#endif
	m_viterbi->nextIteration(_data);
	m_viterbi->forwardStep();
	m_viterbi->backwardStep();
	//_data[0].processing = m_viterbi->getOutput();
}
