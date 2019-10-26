/* Penn Bauman
 * pennbauman@protonmail.com
 */
#ifndef SORTSTACK_H
#define SORTSTACK_H

#include <iostream>

#include "intNode.h"

class sortStack {
	public:
		sortStack();
		~sortStack();

		void push(int a);
		int pop();
		int top();

		void print();

	private:
		bool empty;
		int topVal;
		intNode* next;
};

#endif
