#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "processing.h"


using ::testing::AtLeast;

namespace gtest_filter_custom_velocityfilter
{
	class GTest_filter_custom_velocityfilter : public ::testing::Test
	{
		protected:
			GTest_filter_custom_velocityfilter(){}
			~GTest_filter_custom_velocityfilter() override {}
			void SetUp() override{}
			void TearDown() override {}

	};
	
}  // namespace gtest_filter_custom_velocityfilter
