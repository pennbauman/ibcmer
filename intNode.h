/* Penn Bauman
 * pennbauman@protonmail.com
 */
#ifndef INTNODE_H
#define INTNODE_H

#include <iostream>

class intNode {
	public:
		intNode();
		intNode(int i);
	
	private:
		int val;
		intNode* next;
	
	friend class sortStack;
};

#endif
