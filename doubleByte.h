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
		// Constructors
		doubleByte();
		doubleByte(unsigned short int a);
		doubleByte(string hex);
		~doubleByte();

		// Mutators
		void setVal(unsigned short int a);
		void setVal(string hex);

		// Accessors
		unsigned short int uint() const;
		signed short int sint() const;
		string str() const;
		char cat(int i) const;
		bool bat(int i) const;
		unsigned int addr() const;
		char ascii() const;
		void print() const;
		
		// Operators
		void shiftLeft(int n, bool wrap);
		void shiftRight(int n, bool wrap);
		void add(unsigned short int a);
		void sub(unsigned short int a);
		void band(unsigned short int a);
		void bor(unsigned short int a);
		void bxor(unsigned short int a);
		void bnot();

	private:
		// Values
		bool val[16];
};

#endif
