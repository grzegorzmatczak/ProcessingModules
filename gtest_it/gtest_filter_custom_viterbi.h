#include "gtest/gtest.h"
#include "gmock/gmock.h" 

#include <QDebug>

#include "graph.h"
#include "data.h"
#include "includespdlog.h"
#include "configreader.h"

using ::testing::AtLeast;

namespace gtest_filter_custom_viterbi
{
	class GTest_filter_custom_viterbi : public ::testing::Test
	{
		protected:
			GTest_filter_custom_viterbi(){}
			~GTest_filter_custom_viterbi() override {}
			void SetUp() override{}
			void TearDown() override {}
			QJsonObject readConfig(QString name);
			QJsonArray readArray(QString name);

	};
	
}  // namespace gtest_filter_custom_viterbi
