#include "custom_none.h"

Filters::None::None()
{
	#ifdef DEBUG_BASE_FILTER
		Logger->debug("Filters::None::None()");
	#endif
}

void Filters::None::process(std::vector<_data> &_data)
{
	#ifdef DEBUG_BASE_FILTER
		Logger->debug("Filters::None::process()");
	#endif
}
