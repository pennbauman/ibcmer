/* Penn Bauman
 * pennbauman@protonmail.com
 */
#include "sortStack.h"
using namespace std;

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

void sortStack::push(int a) {
	if (empty) {
		topVal = a;
		empty = false;
	} else {
		if (a == topVal) 
			return;
		if (a < topVal) {
			intNode* temp = new intNode(topVal);
			temp->next = next;
			next = temp;
			topVal = a;
		} else {
			if (a == next->val)
				return;
			if (a < next->val) {
				intNode* temp = new intNode(a);
				temp->next = next;
				next = temp;
			} else {
				if (next->next == NULL) {
					next->next = new intNode(a);
					return;
				}
				intNode* temp = next;
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
				intNode* temp2 = new intNode(a);
				temp2->next = temp->next;
				temp->next = temp2;
			}
		}
	}
}
int sortStack::pop() {
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
