#include "gtest_filter_median_impl_2.h"

using ::testing::AtLeast;

namespace gtest_filter_custom_median_impl_2
{
	constexpr auto CONFIG{"GeneticOptimizationModule/gtest/test_addmultipledron.json"};
	constexpr auto TEST_DATA{"TestData"};
	void GTest_filter_custom_median_impl_2::checkPointers(std::vector<int> m_vector, Subtractors::Node *p)
	{
		EXPECT_EQ(p->key, m_vector[0]);
		for(int i = 1 ; i < m_vector.size() ; i++)
		{
			////Logger->debug("i:{}", i);
			EXPECT_EQ(p->next->key, m_vector[i]);
			p = p->next;
		}
		EXPECT_EQ(p->key, m_vector[ m_vector.size()-1]);
		for(int i =  m_vector.size()-2 ; i > 0; i--)
		{
			//Logger->debug("i:{}", i);
			EXPECT_EQ(p->prev->key, m_vector[i]);
			p = p->prev;
		}
	}

	TEST_F(GTest_filter_custom_median_impl_2, test_medianImage)
	{
		//Logger->set_level(static_cast<spdlog::level::level_enum>(2));
	

		cv::Mat img0 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(0));
		Subtractors::MedianImage medianImage(10);
		medianImage.initMedian(img0);

		cv::Mat median1 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage.getMedianImage(median1);

		

		cv::Mat img1 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(123));
		medianImage.printVector();

		medianImage.addImage(img1);
		medianImage.addImage(img1);
		medianImage.addImage(img1);

		cv::Mat median2 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage.getMedianImage(median2);

		cv::Mat img2 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(125));
		medianImage.addImage(img2);
		medianImage.addImage(img2);
		medianImage.addImage(img2);

		medianImage.printVector();

		cv::Mat median3 = cv::Mat(2, 2, CV_8UC1, cv::Scalar(150));
		medianImage.getMedianImage(median3);

	}

	TEST_F(GTest_filter_custom_median_impl_2, test1)
	{
		//Logger->set_level(static_cast<spdlog::level::level_enum>(2));
		srand((unsigned)time(0));

		Subtractors::MedianList lst(6, 0);

		std::vector<int> m_vector{1,2,3,4,5,6};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 3);


		lst.removeNode();
		//Logger->debug("removeNode 1");
		lst.printMedianList();

		m_vector={2,3,4,5,6};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.m_medianNode->key, 2);
		EXPECT_EQ(lst.getMedian(), 4);

		lst.insertNode(150);
		//Logger->debug("insertNode 150");
		lst.printMedianList();
		
		m_vector={2,3,4,5,6,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 4);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 2);
		EXPECT_EQ(lst.m_stop->prev->key, 6);
		EXPECT_EQ(lst.m_medianNode->key, 2);

		lst.removeNode();
		//Logger->debug("removeNode 2");
		lst.printMedianList();

		m_vector={3,4,5,6,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 5);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 3);
		EXPECT_EQ(lst.m_stop->prev->key, 6);
		EXPECT_EQ(lst.m_medianNode->key, 2);

		lst.insertNode(1);
		//Logger->debug("insertNode 1");
		lst.printMedianList();

		m_vector={1,3,4,5,6,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 4);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 6);

		EXPECT_EQ(lst.m_medianNode->key, 1);

		lst.removeNode();
		//Logger->debug("removeNode 3");
		lst.printMedianList();

		m_vector={1,4,5,6,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 5);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 6);

		EXPECT_EQ(lst.m_medianNode->key, 1);

		lst.insertNode(100);
		//Logger->debug("insertNode 100");
		lst.printMedianList();

		m_vector={1,4,5,6,100,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 5);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 100);


		EXPECT_EQ(lst.m_medianNode->key, 1);

		lst.removeNode();
		//Logger->debug("removeNode 4");
		lst.printMedianList();

		m_vector={1,5,6,100,150};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 6);

		EXPECT_EQ(lst.m_stop->key, 150);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 100);

		EXPECT_EQ(lst.m_medianNode->key, 1);

		lst.insertNode(170);
		//Logger->debug("insertNode 170");
		lst.printMedianList();

		m_vector={1,5,6,100,150,170};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 6);


		EXPECT_EQ(lst.m_stop->key, 170);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 150);

		EXPECT_EQ(lst.m_medianNode->key, 1);

		lst.removeNode();
		//Logger->debug("removeNode 5");
		lst.printMedianList();

		m_vector={1,6,100,150,170};
		checkPointers(m_vector, lst.m_start);
		EXPECT_EQ(lst.getMedian(), 100);

		EXPECT_EQ(lst.m_stop->key, 170);
		EXPECT_EQ(lst.m_stop->next->key, 1);
		EXPECT_EQ(lst.m_stop->prev->key, 150);

		EXPECT_EQ(lst.m_medianNode->key, 1);
	}

	TEST_F(GTest_filter_custom_median_impl_2, test2)
	{
		//Logger->set_level(static_cast<spdlog::level::level_enum>(2));
		Subtractors::imgMedian lst(6);

		cv::Mat img0 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(0));
		lst.InitMedian(img0);
		//Logger->debug("init 0");
		lst.ShowIterator();
		lst.ShowMedianNode(1);

		cv::Mat img1 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(122));
		//Logger->debug("insert 122");
		lst.RemoveNode();
		lst.AddImage(img1);
		lst.ShowMedianNode(1);

		cv::Mat img2 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(200));
		lst.RemoveNode();
		lst.AddImage(img2);
		//Logger->debug("insert 200");
		lst.ShowIterator();
		lst.ShowMedianNode(1);

		cv::Mat img3 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(255));
		lst.RemoveNode();
		lst.AddImage(img3);
		//Logger->debug("insert 255");
		lst.ShowIterator();
		lst.ShowMedianNode(1);

		cv::Mat img4 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(44));
		lst.RemoveNode();
		lst.AddImage(img4);
		//Logger->debug("insert 44");
		lst.ShowIterator();
		lst.ShowMedianNode(1);

		cv::Mat img5 = cv::Mat(5, 5, CV_8UC1, cv::Scalar(100));
		lst.RemoveNode();
		lst.AddImage(img5);
		//Logger->debug("insert 100");

		lst.getMedian(img5);

		lst.ShowIterator();
		lst.ShowMedianNode(1);

	}
	
}  // namespace gtest_filter_custom_median_impl_2
