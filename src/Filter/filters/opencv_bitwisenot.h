#ifndef FILTER_OPENCV_BITWISE_NOT
#define FILTER_OPENCV_BITWISE_NOT

#include "basefilter.h"

class QJsonObject;

namespace Filters
{
	class BitwiseNot : public BaseFilter
	{
		public:
			BitwiseNot(QJsonObject const &a_config);

			void process(std::vector<_data> &_data);
	};

} // namespace Filters

#endif // FILTER_OPENCV_BITWISE_NOT
