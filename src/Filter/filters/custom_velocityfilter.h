#ifndef CUSTOM_VELOCITY_FILTER_H
#define CUSTOM_VELOCITY_FILTER_H

#include "basefilter.h"
#include "viterbi/velocityfilter_impl.h"

#include <vector>

class QJsonObject;

namespace Filters {
class VelocityFilter : public BaseFilter {
 public:
	VelocityFilter(QJsonObject const &a_config);
	~VelocityFilter();

	void process(std::vector<_data> &_data);

 private:
	viterbi::VelocityFilter_impl *m_viterbi;
};
} // namespace Filters

#endif // CUSTOM_VELOCITY_FILTER_H
