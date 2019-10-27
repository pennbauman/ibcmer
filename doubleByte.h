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

		void setVal(unsigned short int a);
		void setVal(string hex);
		void print() const;

		unsigned short int uint() const;
		signed short int sint() const;
		string str() const;
		char cat(int i) const;
		bool bat(int i) const;
		unsigned int addr() const;
		char ascii() const;
		
		void shiftLeft(int n, bool wrap);
		void shiftRight(int n, bool wrap);
		void add(unsigned short int a);
		void sub(unsigned short int a);
		void band(unsigned short int a);
		void bor(unsigned short int a);
		void bxor(unsigned short int a);
		void bnot();

	private:
		bool val[16];
};

#endif
