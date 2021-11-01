#ifndef SUBTRACTORS_CUSTOM_MEDIAN_IMPLEMENTATION_2_H
#define SUBTRACTORS_CUSTOM_MEDIAN_IMPLEMENTATION_2_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>

#include "basesubtractor.h"


using namespace std;

namespace Subtractors
{

	// struct to implement node
	struct Node
	{
	public:
		unsigned char key;
		Node* next;
		Node* prev;
		int index;
	};


	class MedianList
	{

	public:
		MedianList(int listSize, unsigned char initValue);
		~MedianList();
		void printMedianList();
		void removeNode();
		void insertNode(unsigned char key);
		unsigned char getMedian();
		unsigned char getElement(int element);

		Node* m_start;
		Node* m_stop;
		std::vector<Node*> m_vector;
		int m_size;
		int m_vectorSize;
		int m_currentSize;
		Node* m_last_inserted;
		Node* m_last_removed;
		int m_index_to_remove;
		Node* m_medianNode;

	private:

	};

	class MedianImage
	{

	public:
		MedianImage(int listSize);
		~MedianImage();
		void initMedian(cv::Mat &img);
		void addImage(cv::Mat &img);
		void getMedianImage(cv::Mat &img);
		void printVector();

	private:
		unsigned int m_width{};
		unsigned int m_height{};
		unsigned int m_len{};
		unsigned int m_listSize{};
		std::vector<MedianList*> m_medianList;
	};


} // namespace Subtractors

#endif /* SUBTRACTORS_CUSTOM_MEDIAN_IMPLEMENTATION_2_H */
