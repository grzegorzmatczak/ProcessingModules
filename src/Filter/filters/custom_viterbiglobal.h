#ifndef CUSTOM_VITERBI_GLOBAL_H
#define CUSTOM_VITERBI_GLOBAL_H

#include "basefilter.h"
#include "viterbi/viterbiglobal_impl.h"

#include <vector>

class QJsonObject;

namespace Filters {
class ViterbiGlobal : public BaseFilter
{
 public:
	ViterbiGlobal(QJsonObject const &a_config);
	~ViterbiGlobal();

	void process(std::vector<_data> &_data);

 private:
	viterbi::ViterbiGlobal_impl *m_viterbi;

};
} // namespace Filters

#endif // CUSTOM_VITERBI_GLOBAL_H
