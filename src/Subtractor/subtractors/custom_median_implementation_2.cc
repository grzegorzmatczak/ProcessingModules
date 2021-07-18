#include "custom_median_implementation_2.h"

//#define DEBUG
//#define DEBUG_MEDIAN_LIST

using namespace std;

namespace Subtractors
{
	MedianImage::MedianImage(int listSize) :
	m_listSize(listSize)
	{
		#ifdef DEBUG_MEDIAN_LIST
		Logger->debug("MedianImage::MedianImage()");
		#endif
	}

	MedianImage::~MedianImage() 
	{
		#ifdef DEBUG_MEDIAN_LIST
		Logger->debug("MedianImage::~MedianImage()");
		#endif
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				unsigned int i = x + y * m_width;
				delete m_medianList[i];
			}
		}
	}

	void MedianImage::initMedian(cv::Mat &img)
	{
		#ifdef DEBUG_MEDIAN_LIST
		Logger->debug("MedianImage::initMedian()");
		#endif
		m_width = static_cast<unsigned int>(img.cols);
		m_height = static_cast<unsigned int>(img.rows);
		m_len = static_cast<unsigned int>(m_width * m_height);

		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				m_medianList.push_back(new MedianList(m_listSize,0));
			}
		}
	}

	void MedianImage::addImage(cv::Mat &img)
	{
		#ifdef DEBUG_MEDIAN_LIST
		Logger->debug("MedianImage::addImage()");
		#endif
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				unsigned int i = x + y * m_width;
				
				unsigned char val = img.at<unsigned char>(cv::Point(static_cast<int>(x), static_cast<int>(y)));
				#ifdef DEBUG
				Logger->debug("MedianImage::getMedianImage() i:{}, x:{}, y:{}, val:{}", i, x, y, val);
				#endif
				m_medianList[i]->removeNode();
				m_medianList[i]->insertNode(val);
			}
		}
	}

	void MedianImage::getMedianImage(cv::Mat &img)
	{
		#ifdef DEBUG_MEDIAN_LIST
		Logger->debug("MedianImage::getMedianImage()");
		#endif
		for (int y = 0; y < m_height; y++)
		{
			for (int x = 0; x < m_width; x++)
			{
				unsigned int i = x + y * m_width;	
				#ifdef DEBUG
				Logger->debug("MedianImage::getMedianImage() i:{}, x:{}, y:{}:img_val:{}", i, x, y,m_medianList[i]->getMedian());		
				#endif
				img.at<unsigned char>(cv::Point(static_cast<int>(x), static_cast<int>(y))) = m_medianList[i]->getMedian();
			}
		}
	}

	void MedianImage::printVector()
	{
		for (int y = 0; y < m_medianList.size(); y++)
		{
			m_medianList[y]->printMedianList();
		}
	}
		
	MedianList::MedianList(int listSize, unsigned char initValue)
	: m_size(listSize)
	, m_vectorSize(0)
	, m_currentSize(0)
	{   
		for(int i = 0 ; i < listSize ; i++)
		{
			initValue++;
			m_vector.push_back(new Node{initValue, nullptr, nullptr, i});
		}
		#ifdef DEBUG
		Logger->debug("m_vector.size():{}", m_vector.size());
		Logger->debug("m_size:{}", m_size);
		#endif
		m_vectorSize = m_vector.size() - 1;
		m_currentSize = m_vector.size();

		for(int i = 0; i < m_vector.size(); ++i)
		{
			if (i < (m_vectorSize))
				m_vector[i]->next = m_vector[i+1];
			if (i == (m_vectorSize))
				m_vector[i]->next = m_vector[0];

			if (i > 0)
				m_vector[i]->prev = m_vector[i-1];
			if (i == 0)
				m_vector[i]->prev = m_vector[m_vectorSize];
		}
		m_start = m_vector[0];
		m_medianNode = m_vector[1];
		m_stop = m_vector[m_vectorSize];
		m_index_to_remove =  0;
	};
	MedianList::~MedianList() 
	{
		#ifdef DEBUG
		Logger->debug("MedianList::~MedianList()");
		#endif
		for(int i = 0 ; i < m_size ; i++)
		{
			delete m_vector[i];
		}
	}

	unsigned char MedianList::getMedian()
	{
		if (m_currentSize % 2 != 0)
		{
			return getElement(m_currentSize/2);
		}
		return getElement((m_currentSize-1)/2);
	}

	unsigned char MedianList::getElement(int element)
	{
		#ifdef DEBUG
		Logger->debug("MedianList::getElement()");
		#endif
		Node * node = m_start;
		int iteration{0};
		while(true)
		{
			if(iteration >= element)
				break;
			iteration++;
			node = node->next;
		}
		#ifdef DEBUG
		Logger->debug("MedianList::getElement() return:{}", node->key);
		#endif
		return node->key;
	}

	void MedianList::printMedianList()
	{  
		Node* currentNode = m_start;

		for(int i=0; i<m_vector.size(); ++i)
		{
			#ifdef DEBUG
			Logger->debug("MedianList::printMedianList() next {}", currentNode->key);
			#endif
			currentNode = currentNode->next;
		}
		currentNode = m_stop;
		for(int i=m_vector.size() ; i>=0 ; --i)
		{
			#ifdef DEBUG
			Logger->debug("MedianList::printMedianList() prev {}", currentNode->key);
			#endif
			currentNode = currentNode->prev;
		}
	}

	void MedianList::removeNode()
	{
		m_currentSize--;
		Node* nodeToDelete = m_vector[m_index_to_remove];
		if (nodeToDelete == m_start)
		{
			m_start = m_start->next;
			m_stop->next = m_start;
		}
		if (nodeToDelete == m_stop)
		{
			m_stop = m_stop->prev;
		}

		Node* next_node = nodeToDelete->next;
		Node* prev_node = nodeToDelete->prev;

		next_node->prev = nodeToDelete->prev;
		prev_node->next = nodeToDelete->next;
	}

	void MedianList::insertNode(unsigned char key)
	{
		m_currentSize++;
		Node* nodeToAdd = m_vector[m_index_to_remove];
		m_index_to_remove++;
		if(m_index_to_remove >= m_vectorSize)
		{
			m_index_to_remove = 0;
		}

		if(key <= m_start->key)
		{
			// add as a m_start:
			nodeToAdd->prev = m_start->prev;
			nodeToAdd->next = m_start;
			nodeToAdd->key = key;

			m_start->prev = nodeToAdd;
			m_stop->next = nodeToAdd;
			m_start = nodeToAdd;
			return;
		}
		if(key >= m_stop->key)
		{
			nodeToAdd->prev = m_start->prev;
			nodeToAdd->next = m_start;
			nodeToAdd->key = key;

			m_stop->next = nodeToAdd;
			m_start->prev = nodeToAdd;
			m_stop = nodeToAdd;
			return;
		}

		Node * node = m_start;

		while(true)
		{
			if(key >= node->key && key <= node->next->key)
			{
				Node* next_node = node->next;
				nodeToAdd->key = key;
				nodeToAdd->next = next_node;
				nodeToAdd->prev = node;

				next_node->prev = nodeToAdd;
				node->next = nodeToAdd;
				break;
			}
			else
			{
				node = node->next;
			}
		}
	}

} // namespace Subtractors
