/* Penn Bauman
 * pennbauman@protonmail.com
 */
#include "intNode.h"
using namespace std;

intNode::intNode() {
	val = 0;
	next = NULL;
}
intNode::intNode(int i) {
	val = i;
	next = NULL;
}
intNode::~intNode() {
	//cout << "x" << endl;
}
