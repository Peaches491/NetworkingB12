/*
 * LPMTree.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: cs3516
 */

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include "LPMTree.h"

using namespace std;

void LPMTree::insert(int queueNum, uint32_t ip, int size) {
	this->insert(queueNum, ip, size, 0);
}

void LPMTree::insert(int queueNum, uint32_t ip, int size, int currDepth) {
	if (size <= currDepth) {
		//cout << "Storing number " << queueNum << " and returning." << endl;
		this->queue = queueNum;
		return;
	}
	bool nextDigit = 0x80000000 & (ip << currDepth);
	//bool nextDigit = 0b1000 & (ip << currDepth);
	//cout << "Next Digit is a " << (int) nextDigit << endl;

	LPMTree* node;
	if (nextDigit) { // if the next digit is a 'one'
		if (this->one == NULL) {
			this->one = new LPMTree();
		}
		node = this->one;
		//cout << "Moving to child one" << endl;
	} else {
		if (this->zero == NULL) {
			this->zero = new LPMTree();
		}
		node = this->zero;
		//cout << "Moving to child zero" << endl;
	}

	//cout << "Recursing" << endl << endl;
	node->insert(queueNum, ip, size, currDepth + 1);
}

int LPMTree::get(uint32_t ip, int size) {
	return this->get(ip, size, 0);
}

int LPMTree::get(uint32_t ip, int size, int currDepth) {

	int data = -1;
	int newData = -1;

	data = this->queue;
	//cout << "At depth: " << currDepth << " the data is a: " << data << endl;

	if (size <= currDepth) {
		//cout << "Reched maximum depth. Found a: " << data << endl;
		return data;
	}

	bool nextDigit = 0x80000000 & (ip << currDepth);
	//bool nextDigit = 0b1000 & (ip << currDepth);
	//cout << "Next Digit is a " << (int) nextDigit << endl;

	LPMTree* node;
	if (nextDigit) { // if the next digit is a 'one'
		node = this->one;
		//cout << "Moving to child one" << endl << endl;
	} else {
		node = this->zero;
		//cout << "Moving to child zero" << endl << endl;
	}

	if (node == NULL) {
		//cout << "Node was null. Returning." << endl;
		return -1;
	}

	newData = node->get(ip, size, currDepth + 1);

	//cout << "Data: " << data << " \tNew: " << newData << endl;

	if (newData != -1)
		return newData;
	else
		return data;
}

void LPMTree::print() {
	this->print(string(""));
}

void LPMTree::print(string str) {

	cout << str;

	if (this->zero != NULL) {
		str.append("0");
		cout << "\t" << this->queue << endl;
		this->zero->print(str);
	}


	if (this->one != NULL) {
		str.append("1");
		cout << "\t" << this->queue << endl;
		this->one->print(str);
	}
	return;
}

LPMTree::LPMTree() {
	one = NULL;
	zero = NULL;
	queue = -1;
}

LPMTree::~LPMTree() {
	if (this->one != NULL)
		delete this->one;
	if (this->zero != NULL)
		delete this->zero;
	free(this);
}

