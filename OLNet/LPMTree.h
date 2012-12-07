/*
 * LPMTree.h
 *
 *  Created on: Dec 5, 2012
 *      Author: cs3516
 */

#ifndef LPMTREE_H_
#define LPMTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>

namespace std {

class LPMTree {
private:
	LPMTree* one;
	LPMTree* zero;
	int queue;

	void print(string depth);
	int  get(uint32_t ip, int size, int currDepth);
public:
	LPMTree();
	bool isEmpty() const {
		return (one == NULL) && (zero == NULL);
	}

	void insert(int queueNum, uint32_t ip, int size, int currDepth);
	void insert(int queueNum, uint32_t ip, int size);
	void insert(int queueNum, in_addr addr, int size);
	void insert(int queueNum, char* ipString, int size);
	int  get(uint32_t ip, int size);
	int  get(char* ipString, int strLen, int size);
	int  get(in_addr addr, int size);
	void print();

	virtual ~LPMTree();
};

} /* namespace std */
#endif /* LPMTREE_H_ */

