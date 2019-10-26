/* Penn Bauman
 * pennbauman@protonmail.com
 */
#ifndef DOUBLEBYTE_H
#define DOUBLEBYTE_H

#include <iostream>
#include <string>
#include <cmath>
using namespace std;

class doubleByte {
	public:
		doubleByte();
		doubleByte(unsigned short int a);
		doubleByte(signed short int a);
		doubleByte(string hex);
		~doubleByte();

		unsigned short int uint() const;
		signed short int sint() const;
		string str() const;
		char cat(int i) const;
		bool bat(int i) const;
		unsigned int addr() const;
		char ascii() const;
		
		void shiftLeft(int n, bool wrap);
		void shiftRight(int n, bool wrap);
		unsigned short int add(unsigned short int a);
		unsigned short int sub(unsigned short int a);
		unsigned short int band(unsigned short int a);
		unsigned short int bor(unsigned short int a);
		unsigned short int bxor(unsigned short int a);
		unsigned short int bnot();

		void print() const;

	private:
		bool val[16];
};

#endif
