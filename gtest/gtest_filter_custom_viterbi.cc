#include "gtest_filter_custom_viterbi.h"

#include "filters/basefilter.h"
#include "filter.h"
#include "filterlist.h"
//#include "data.h"
#include "configreader.hpp"
#include <QJsonArray>
#include <QJsonObject>



#define DEBUG_OPENCV

using ::testing::AtLeast;


namespace gtest_filter_custom_viterbi
{
	constexpr auto CONFIG{"ProcessingModules/gtest/test_viterbi.json"};
	constexpr auto TEST_DATA{"TestData"};
	TEST_F(GTest_filter_custom_viterbi, test_viterbi_contructor)
	{
		/*
		//Logger->set_level(static_cast<spdlog::level::level_enum>(0));
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonObject jObject;
		if (!cR->readConfig(CONFIG, jObject))
		{
			//Logger->error("File {} read confif failed", CONFIG);
			EXPECT_EQ(0,1);
		}

		Filters::ViterbiFilter* m_filter = new Filters::ViterbiFilter{ jObject };

		DataMemory* m_dataMemory = new DataMemory();
		m_dataMemory->configure(jObject);
		QJsonArray preprocessConfig = jObject["Graph_estimator_with_filters_small_drons_contrast"].toArray();
		
		cv::Mat input = m_dataMemory->clean(0).clone();
		cv::Mat gt = m_dataMemory->gt(0).clone();


		if(!m_dataMemory->preprocess(preprocessConfig))
		{
			EXPECT_EQ(0,1);
		}

		for (int iteration = 0; iteration < m_dataMemory->getSizeCleanTrain(); iteration++)
		{
			cv::Mat input = m_dataMemory->cleanTrain(iteration).clone();
			cv::Mat gt = m_dataMemory->gtTrain(iteration).clone();
			_data data{input, "TestString"};
			std::vector<_data> _dataVector{data};
			
			try
			{
				m_filter->process(_dataVector);
			}
			catch (cv::Exception& e)
			{
				const char* err_msg = e.what();
				qDebug() << "exception caught: " << err_msg;
			}
			#ifdef DEBUG_OPENCV
			cv::Mat preview;
			cv::cvtColor(input, preview, cv::COLOR_GRAY2BGR);
			#endif
		}
		*/
	}
	
}  // namespace gtest_filter_custom_viterbi
