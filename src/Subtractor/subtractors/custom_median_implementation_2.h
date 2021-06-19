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

namespace Subtractors {

// struct to implement node
struct Node
{
public:
	unsigned char key;
	Node* next;
	Node* prev;
	int index;
};

// Class for Skip list
class MedianList
{

public:
	MedianList(int listSize, unsigned char initValue);
	void printMedianList();
	void removeNode();
	void insertNode(unsigned char key);

	Node* m_start;
	Node* m_stop;
	std::vector<Node*> m_vector;
	int m_size;
	Node* m_last_inserted;
	Node* m_last_removed;
	int m_index_to_remove;
	Node* m_medianNode;

private:

};


MedianList::MedianList(int listSize, unsigned char initValue)
{   
	for(int i = 0 ; i < listSize ; i++)
	{
		initValue++;
		m_vector.push_back(new Node{initValue, nullptr, nullptr, i});
	}
	Logger->debug("m_vector.size():{}", m_vector.size());
	m_size = m_vector.size() - 1;
	

	for(int i = 0; i < m_vector.size(); ++i)
	{
		if (i < (m_size))
			m_vector[i]->next = m_vector[i+1];
		if (i == (m_size))
			m_vector[i]->next = m_vector[0];

		if (i > 0)
			m_vector[i]->prev = m_vector[i-1];
		if (i == 0)
			m_vector[i]->prev = m_vector[m_size];
	}
	m_start = m_vector[0];
	m_medianNode = m_vector[1];
	m_stop = m_vector[m_size];
	m_index_to_remove =  0;
};

void MedianList::printMedianList()
{  
	Node* currentNode = m_start;

	for(int i=0; i<m_vector.size(); ++i)
	{
		currentNode = currentNode->next;
	}
	currentNode = m_stop;
	for(int i=m_vector.size() ; i>=0 ; --i)
	{
		currentNode = currentNode->prev;
	}
}

void MedianList::removeNode()
{
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
	Node* nodeToAdd = m_vector[m_index_to_remove];
	m_index_to_remove++;
	if(m_index_to_remove >= m_size)
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
		if(key >= node->key and key <= node->next->key)
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

#endif /* SUBTRACTORS_CUSTOM_MEDIAN_IMPLEMENTATION_2_H */
