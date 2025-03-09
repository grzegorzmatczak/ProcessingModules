#include "gtest_filter_custom_addmultipledron.h"

#include "filters/basefilter.h"
#include "filter.h"
#include "filterlist.h"
#include "configreader.hpp"

using ::testing::AtLeast;


namespace gtest_filter_custom_addmultipledron {

	constexpr auto CONFIG{"ProcessingModules/gtest/test_addmultipledron.json"};
	constexpr auto TEST_DATA{"TestData"};
	TEST_F(GTest_filter_custom_addmultipledron, test_checkDronList)
	{
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonObject jObject;
		if (!cR->readConfig(CONFIG, jObject))
		{
			//Logger->error("File {} read confif failed", CONFIG);
			//EXPECT_EQ(0,1);
		}

		Filters::AddMultipleDron* m_filter = new Filters::AddMultipleDron{ jObject };
		bool opt1{false};
		bool opt2{false};
		QString whiteBlack = "WHITE_BLACK";
		m_filter->checkDronList(whiteBlack, opt1, opt2);
		EXPECT_EQ(opt1, true);
		EXPECT_EQ(opt2, true);


		QString whiteBlack2 = "WHITE";
		m_filter->checkDronList(whiteBlack2, opt1, opt2);
		EXPECT_EQ(opt1, true);
		EXPECT_EQ(opt2, false);

		QString whiteBlack3 = "BLACK";
		m_filter->checkDronList(whiteBlack3, opt1, opt2);
		EXPECT_EQ(opt1, false);
		EXPECT_EQ(opt2, true);
	}

	TEST_F(GTest_filter_custom_addmultipledron, test2)
	{

	}
	
}  // namespace gtest_filter_custom_addmultipledron
