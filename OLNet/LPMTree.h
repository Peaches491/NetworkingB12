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

namespace std {

class LPMTree {
private:
	LPMTree* one;
	LPMTree* zero;
	int queue;

public:
	LPMTree() {
		one = NULL;
		zero = NULL;
		queue = 0;
	}

	bool isEmpty() const {
		return (one == NULL) && (zero == NULL);
	}
	void print_inorder();
	void inorder(LPMTree*);
	void print_preorder();
	void preorder(LPMTree*);
	void print_postorder();
	void postorder(LPMTree*);
	void insert(int queueNum, uint32_t ip, int size, int currDepth);
	void insert(int queueNum, uint32_t ip, int size);
	void remove(int);
	void print();

	virtual ~LPMTree();
};

} /* namespace std */
#endif /* LPMTREE_H_ */

