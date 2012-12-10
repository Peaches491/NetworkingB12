/*
 * LPMTree.cpp
 *
 *  Created on: Dec 5, 2012
 *      Author: cs3516
 */

#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <math.h>
#include "LPMTree.h"

using namespace std;

void printAddr(string str);

// Expects ip to be big endian.
void LPMTree::insert(int queueNum, uint32_t ip, int size) {

	uint32_t mask = 0xFFFFFFFF;
	mask = (mask << (32-size));
	//printf("%X\n", mask);
	ip = (ip & mask);
	//printf("%8X\n", ip);

	cout << "Inserting: ";
	cout << queueNum << " @ ";
	in_addr addr;
	addr.s_addr = ntohl(ip);
	cout << (inet_ntoa(addr));
	cout << "/" << size << endl;

	this->insert(queueNum, ip, size, 0);
}

void LPMTree::insert(int queueNum, char* ipString, int size) {
	in_addr addr;
	inet_aton(ipString, &addr);
	this->insert(queueNum, ntohl(addr.s_addr), size);
}

void LPMTree::insert(int queueNum, in_addr addr, int size) {

	this->insert(queueNum, ntohl(addr.s_addr), size);
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

int LPMTree::get(in_addr addr, int size) {
	return this->get(ntohl(addr.s_addr), size, 0);
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
	cout << "Data in LPMTree ------------------" << endl;
	this->print(string(""));
	cout << "----------------------------------" << endl;
}

void LPMTree::print(string str) {
	string spacer("");

	//cout << str << endl;
	if (this->zero != NULL) {
		if (this->queue != -1) {
			printAddr(str);
			cout << spacer << this->queue << endl;
		}
		str.append("0");
		this->zero->print(str);
	}

	if (this->one != NULL) {
		if (this->queue != -1) {
			printAddr(str);
			cout << spacer << this->queue << endl;
		}
		str.append("1");
		this->one->print(str);
	}

	if (this->isEmpty()) {
		if (this->queue != -1) {
			printAddr(str);
			cout << spacer << this->queue << endl;
		}
	}

	if (str.length() == 1) {
		cout << endl << endl << endl;
	}
	return;
}

void printAddr(string str) {
	if (false) {
		uint32_t number = 0;
		for (unsigned int i = 0; i < str.length(); i++) {
			uint32_t thisNum;
			if (str[i] == '1'){
				thisNum = 1;
			} else {
				thisNum = 0;
			}

			number = (number + thisNum);
			if (i != 31) number = (number << 1);
		}
		number = (number << (32 - (str.length())));

		in_addr addr;
		addr.s_addr = ntohl(number);
		printf("%-32s ", str.c_str());
		printf("%8X ", number);
		printf("%15s", (inet_ntoa(addr)));
	}
}

LPMTree::LPMTree() {
	one = NULL;
	zero = NULL;
	queue = -1;
}

LPMTree::~LPMTree() {
	//cout << "Deleting ones..." << endl;
	delete (this->one);
	//cout << "Deleting zeros..." << endl;
	delete (this->zero);
	//free(this);
}

