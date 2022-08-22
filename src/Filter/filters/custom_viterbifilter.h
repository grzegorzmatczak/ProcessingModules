#ifndef CUSTOM_VITERBI_FILTER_H
#define CUSTOM_VITERBI_FILTER_H

#include "basefilter.h"
#include "viterbi/viterbifilter_impl.h"

#include <vector>

class QJsonObject;

namespace Filters {
class ViterbiFilter : public BaseFilter
{
 public:
	ViterbiFilter(QJsonObject const &a_config);
	~ViterbiFilter();

	void process(std::vector<_data> &_data);

 private:
	viterbi::ViterbiFilter_impl *m_viterbi;

};
} // namespace Filters

#endif // CUSTOM_VITERBI_FILTER_H
