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
		cout << "Storing number " << queueNum << " and returning." << endl;;
		this->queue = queueNum;
		return;
	}
	//bool nextDigit = 0x80000000 & (ip << currDepth);
	bool nextDigit = 0x8 & (ip << currDepth);
	cout << "Next Digit is a " << (int) nextDigit << endl;

	LPMTree* node;
	if (nextDigit) { // if the next digit is a 'one'
		node = this->one;
		cout << "Moving to child one" << endl;
	} else {
		node = this->zero;
		cout << "Moving to child zero" << endl;
	}

	if (node == NULL) {
		cout << "Child was NULL" << endl;
		node = new LPMTree;
	}
	cout << "Recursing" << endl << endl;
	node->insert(queueNum, ip, size, currDepth + 1);
}

void LPMTree::print() {
	if ((this->zero == NULL) && (this->one == NULL)) {
		cout << "\t\t" << this->queue << endl;
		return;
	}

	if (this->zero != NULL) {
		cout << "0";
		cout.flush();
		this->zero->print();
		cout << endl;
	}

	if (this->one != NULL) {
		cout << "1";
		cout.flush();
		this->one->print();
		cout << endl;
	}
	return;
}

LPMTree::~LPMTree() {
	if (this->one != NULL)
		delete this->one;
	if (this->zero != NULL)
		delete this->zero;
	free(this);
}

