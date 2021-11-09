#include "gtest_filter_custom_viterbi.h"

#include "filters/basefilter.h"
#include "filter.h"
#include "filterlist.h"
#include "data.h"
#include "case.h"
#include "processing.h"
#include "postprocessing.h"

#include <QJsonArray>
#include <QJsonObject>

constexpr auto TEST_BOUNDS{ 		"ProcessingModules/gtest_it/test_bounds.json" };
constexpr auto TEST_CONFIG{ 		"ProcessingModules/gtest_it/test_config.json" };
constexpr auto TEST_GRAPH{ 			"ProcessingModules/gtest_it/test_graph.json" };
constexpr auto TEST_GRAPH_CONFIG{ 			"ProcessingModules/gtest_it/test_graph_config.json" };
constexpr auto TEST_DATASET{ 		"ProcessingModules/gtest_it/test_dataset.json" };
constexpr auto TEST_PREPROCESS{ 	"ProcessingModules/gtest_it/test_preprocess.json" };
constexpr auto TEST_POSTPROCESS{ 	"ProcessingModules/gtest_it/test_postprocess.json" };


constexpr auto GRAPH{ "Graph_estimator_viterbi" };

constexpr auto NAME{ "Name" };
constexpr auto ACTIVE{ "Active" };
constexpr auto COPY_SIGNAL{ "Signal" };
constexpr auto TYPE{ "Type" };
constexpr auto NEXT{ "Next" };
constexpr auto PREV{ "Prev" };
constexpr auto CONFIG{ "Config" };
constexpr auto WIDTH{ "Width" };
constexpr auto HEIGHT{ "Height" };

//#define DEBUG_OPENCV
//#define DEBUG_GRAPH
//#define DEBUG_CONFIG
//#define DEBUG_CONFIG_GRAPH

using ::testing::AtLeast;


namespace gtest_filter_custom_viterbi
{
	QJsonObject GTest_filter_custom_viterbi::readConfig(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonObject jObject;
		if (!cR->readConfig(configName, jObject))
		{
			Logger->error("File {} read config failed", configName.toStdString());
			EXPECT_EQ(0,1);
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jObject <<  "\n";
		#endif
		return jObject;
	}

	QJsonArray GTest_filter_custom_viterbi::readArray(QString name)
	{
		QString configName{ name };
		std::shared_ptr<ConfigReader> cR = std::make_shared<ConfigReader>();
		QJsonArray jarray;
		if (!cR->readConfig(configName, jarray))
		{
			Logger->error("File {} read config failed", configName.toStdString());
			EXPECT_EQ(0,1);
		}
		#ifdef DEBUG_CONFIG
		qDebug() << name << ":" << jarray <<  "\n";
		#endif
		return jarray;
	}

	TEST_F(GTest_filter_custom_viterbi, test_viterbi_contructor)
	{
		Logger->set_level(static_cast<spdlog::level::level_enum>(0));
		Logger->debug("GTest_filter_custom_viterbi() QDir::currentPath():{}", QDir::currentPath().toStdString().c_str());

		QString graphString = "Graph_estimator_viterbi";

		QJsonObject all_bounds = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_BOUNDS));
		QJsonObject all_graph = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_GRAPH));
		QJsonObject all_graph_config = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_GRAPH_CONFIG));
		QJsonObject all_preprocess = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_PREPROCESS));
		QJsonObject all_postprocess = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_POSTPROCESS));
		QJsonObject all_dataset = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_DATASET));

		QJsonObject config = GTest_filter_custom_viterbi::readConfig(QDir::toNativeSeparators(QDir::currentPath()+QDir::separator()+TEST_CONFIG));
		QJsonObject bounds = all_bounds[graphString].toObject();
		QJsonArray preprocess = all_preprocess[graphString].toArray();
		QJsonArray postprocess = all_postprocess[graphString].toArray();
		QJsonObject dataset = all_dataset[graphString].toObject();
		QJsonArray graph = all_graph[graphString].toArray();
		QJsonArray graph_config = all_graph_config[graphString].toArray();

		EXPECT_EQ(false,config.empty());
		EXPECT_EQ(false,bounds.empty());
		EXPECT_EQ(false,preprocess.empty());
		EXPECT_EQ(false,postprocess.empty());
		EXPECT_EQ(false,dataset.empty());
		EXPECT_EQ(false,graph.empty());

		EXPECT_EQ(false,graph_config.empty());

		DataMemory* m_dataMemory = new DataMemory();
		m_dataMemory->configure(dataset);
		if(!m_dataMemory->preprocess(preprocess))
		{
			EXPECT_EQ(0,1);
		}

		Case *m_case = new Case(m_dataMemory);
		fitness fs = m_case->onConfigureAndStart(graph, graph_config, postprocess);
	}	
}  // namespace gtest_filter_custom_viterbi
