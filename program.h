/* Penn Bauman
 * pennbauman@protonmail.com
 */
#ifndef PROGRAM_H
#define PROGRAM_H

#define ADDR 4096
//#define ADDR 128

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stack>

#include "sortStack.h"
#include "doubleByte.h"
using namespace std;

class program {
	public:
		program();
		~program();

		bool init(string codeFile, stack<int>* b, bool strict, bool check);
		int step(bool loud);
		// 0 = halt
		// 1 = success
		// 2 = breakpoint
		// 3 = error

		unsigned int pid();

		void print() const;
		void printMem() const;
		void printMem(unsigned int i) const;
		void printMem(unsigned int i, bool formated) const;

	private:
		unsigned short int mem[ADDR];
		doubleByte acc;
		unsigned short int counter;
		sortStack breaks;
};

bool checkHex(string s);

#endif
