/* Penn Bauman
 * pennbauman@protonmail.com
 */
#include "sortStack.h"
using namespace std;

// Constructors
sortStack::sortStack() {
	empty = true;
	topVal = 0;
	next = NULL;
}
sortStack::~sortStack() {
	while (next != NULL) {
		pop();
	}
}

// Operations
void sortStack::push(int a) {
	if (empty) {
		topVal = a;
		empty = false;
	} else {
		// Check placement at top of the stack
		if (a == topVal)
			return;
		if (a < topVal) {
			intNode* temp = new intNode(topVal);
			temp->next = next;
			next = temp;
			topVal = a;
		} else {
			// Check placement at the second place on the stack
			if (a == next->val)
				return;
			if (a < next->val) {
				intNode* temp = new intNode(a);
				temp->next = next;
				next = temp;
			} else {
				// Check placement at the third place on the stack
				if (next->next == NULL) {
					next->next = new intNode(a);
					return;
				}
				intNode* temp = next;
				// Find place on the stack
				while (a > temp->next->val) {
					temp = temp->next;
					if (temp->next == NULL)
						break;
				}
				if (temp->next == NULL) {
					temp->next = new intNode(a);
					return;
				}
				if (a == temp->next->val)
					return;
				// Insert element
				intNode* temp2 = new intNode(a);
				temp2->next = temp->next;
				temp->next = temp2;
			}
		}
	}
}
int sortStack::pop() {
	if (empty)
		return -1;
	int fin = topVal;
	if (next == NULL) {
		empty = true;
	} else {
		topVal = next->val;
		intNode* temp = next;
		next = next->next;
		delete temp;
	}
	return fin;
}
int sortStack::top() {
	if (empty)
		return -1;
	return topVal;
}
void sortStack::print() {
	cout << "sortStack: ";
	if (empty) {
		cout << "empty ";
	} else {
		cout << dec << topVal << " ";
		intNode* temp = next;
		while (temp != NULL) {
			cout << temp->val << " ";
			temp = temp->next;
		}
	}
	cout << endl;
}
