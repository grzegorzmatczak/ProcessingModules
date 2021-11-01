#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "configreader.h"
#include "includespdlog.h"
#include "custom_median_implementation.h"
#include "custom_median_implementation_2.h"
#include "processing.h"


using ::testing::AtLeast;

namespace gtest_filter_custom_median_impl_2
{
	class GTest_filter_custom_median_impl_2 : public ::testing::Test
	{
		protected:
			GTest_filter_custom_median_impl_2(){}
			~GTest_filter_custom_median_impl_2() override {}
			void SetUp() override{}
			void TearDown() override {}
		
		public:
			void checkPointers(std::vector<int> m_vector, Subtractors::Node *p);

	};
	
}  // namespace gtest_filter_custom_median_impl_2
