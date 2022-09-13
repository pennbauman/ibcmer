// Double Byte Class - IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#ifndef DOUBLEBYTE_H
#define DOUBLEBYTE_H

#include <iostream>
#include <string>
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
		// Powers of 2
		unsigned short int pow2[16] = {32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1};

		// Values
		bool val[16];
};

#endif
