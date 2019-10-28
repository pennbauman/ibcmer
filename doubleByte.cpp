/* Penn Bauman
 * pennbauman@protonmail.com
 */
#include "doubleByte.h"
using namespace std;

// Constructors
doubleByte::doubleByte() {
	for (int i = 0; i < 16; i++) {
		val[i] = false;
	}
}
doubleByte::doubleByte(unsigned short int a) {
	setVal(a);
}
doubleByte::doubleByte(string hex) {
	setVal(hex);
}
doubleByte::~doubleByte() {}

// Mutators
void doubleByte::setVal(unsigned short int a) {
	for (int i = 0; i < 16; i++) {
		if (a >= pow(2,(15-i))) {
			val[i] = true;
			a -= pow(2,(15-i));
		} else {
			val[i] = false;
		}
	}
}
void doubleByte::setVal(string hex) {
	while (hex.size() < 4)
		hex = "0" + hex;
	for (int i = 0; i < 4; i++) {
		switch(hex.at(i)) {
			case '0':
				val[(4*i)+0] = false;
				val[(4*i)+1] = false;
				val[(4*i)+2] = false;
				val[(4*i)+3] = false;
				break;
			case '1':
				val[(4*i)+0] = false;
				val[(4*i)+1] = false;
				val[(4*i)+2] = false;
				val[(4*i)+3] = true;
				break;
			case '2':
				val[(4*i)+0] = false;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case '3':
				val[(4*i)+0] = false;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			case '4':
				val[(4*i)+0] = false;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = false;
				break;
			case '5':
				val[(4*i)+0] = false;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = true;
				break;
			case '6':
				val[(4*i)+0] = false;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case '7':
				val[(4*i)+0] = false;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			case '8':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = false;
				val[(4*i)+3] = false;
				break;
			case '9':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = false;
				val[(4*i)+3] = true;
				break;
			case 'a':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case 'b':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			case 'c':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = false;
				break;
			case 'd':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = true;
				break;
			case 'e':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case 'f':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			case 'A':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case 'B':
				val[(4*i)+0] = true;
				val[(4*i)+1] = false;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			case 'C':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = false;
				break;
			case 'D':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = false;
				val[(4*i)+3] = true;
				break;
			case 'E':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = false;
				break;
			case 'F':
				val[(4*i)+0] = true;
				val[(4*i)+1] = true;
				val[(4*i)+2] = true;
				val[(4*i)+3] = true;
				break;
			default:
				cerr << "ERROR: Unknown hex while making doubleByte." << endl;
		}
	}
}

// Accessors
unsigned short int doubleByte::uint() const { // unsigned int
	unsigned short int fin = 0;
	for (int i = 0; i < 16; i++) {
		fin += val[i]*pow(2,(15-i));
	}
	return fin;
}
signed short int doubleByte::sint() const { // signed int
	signed short int fin = 0;
	if (val[0])
		fin -= 32768;
	for (int i = 1; i < 16; i++) {
		fin += val[i]*pow(2,(15-i));
	}
	return fin;
}
string doubleByte::str() const { // hexidecimal string
	string fin = "";
	fin += cat(0);
	fin += cat(1);
	fin += cat(2);
	fin += cat(3);
	return fin;
}
char doubleByte::cat(int i) const { // char at index
	if ((i < 0) || (i > 3)) {
		cerr << "ERROR: Improper char index." << endl;
		return '!';
	}
	if (val[(4*i)+0]) {
		if (val[(4*i)+1]) {
			if (val[(4*i)+2]) {
				if (val[(4*i)+3]) {
					return 'f';
				} else {
					return 'e';
				}
			} else {
				if (val[(4*i)+3]) {
					return 'd';
				} else {
					return 'c';
				}
			}
		} else {
			if (val[(4*i)+2]) {
				if (val[(4*i)+3]) {
					return 'b';
				} else {
				 return 'a';
				}
			} else {
				if (val[(4*i)+3]) {
					return '9';
				} else {
					return '8';
				}
			}
		}
	} else {
		if (val[(4*i)+1]) {
			if (val[(4*i)+2]) {
				if (val[(4*i)+3]) {
					return '7';
				} else {
					return '6';
				}
			} else {
				if (val[(4*i)+3]) {
					return '5';
				} else {
					return '4';
				}
			}
		} else {
			if (val[(4*i)+2]) {
				if (val[(4*i)+3]) {
					return '3';
				} else {
					return '2';
				}
			} else {
				if (val[(4*i)+3]) {
					return '1';
				} else {
					return '0';
				}
			}
		}
	}
}
bool doubleByte::bat(int i) const { // boolean at index
	if ((i < 0) || (i > 15)) {
		cerr << "ERROR: Improper char index." << endl;
		return false;
	}
	return val[i];
}
unsigned int doubleByte::addr() const { // address component of doubleByte
	unsigned int fin = 0;
	for (int i = 4; i < 16; i++) {
		fin += val[i]*pow(2,(15-i));
	}
	return fin;
}
char doubleByte::ascii() const { // ascii component of doubleByte
	char fin = 0;
	for (int i = 8; i < 16; i++) {
		fin += val[i]*pow(2,(15-i));
	}
	return fin;
	
}
void doubleByte::print() const { // print binary repersentation of doubleByte
	for (int i = 0; i < 16; i++) {
		cout << val[i];
		if ((i+1)%4 == 0)
			cout << " ";
	}
	cout << endl;
}

// Operators
void doubleByte::shiftLeft(int n, bool wrap) {
	for (int i = 0; i < n; i++) {
		bool temp = val[0];
		for (int j = 0; j < 15; j++) {
			val[j] = val[j+1];
		}
		if (wrap)
			val[15] = temp;
	}
}
void doubleByte::shiftRight(int n, bool wrap) {
	for (int i = 0; i < n; i++) {
		bool temp = val[15];
		for (int j = 15; j > 1; j--) {
			val[j] = val[j-1];
		}
		if (wrap)
			val[0] = temp;
	}
}
void doubleByte::add(unsigned short int a) {
	setVal(uint() + a);
}
void doubleByte::sub(unsigned short int a) {
	setVal(uint() - a);
}
void doubleByte::band(unsigned short int a) {
	setVal(uint() & a);
}
void doubleByte::bor(unsigned short int a) {
	setVal(uint() | a);
}
void doubleByte::bxor(unsigned short int a) {
	setVal(uint() ^ a);
}
void doubleByte::bnot() {
	setVal(~uint());
}

