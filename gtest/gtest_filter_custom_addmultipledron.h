#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "configreader.h"
#include "includespdlog.h"

#include "processing.h"


using ::testing::AtLeast;

namespace gtest_filter_custom_addmultipledron
{
	class GTest_filter_custom_addmultipledron : public ::testing::Test
	{
		protected:
			GTest_filter_custom_addmultipledron(){}
			~GTest_filter_custom_addmultipledron() override {}
			void SetUp() override{}
			void TearDown() override {}

	};
	
}  // namespace gtest_filter_custom_addmultipledron
