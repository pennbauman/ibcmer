// Program Class - IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#ifndef PROGRAM_H
#define PROGRAM_H

#define ADDR 4096

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <stack>
#include <unordered_set>

#include "doubleByte.h"
using namespace std;

class program {
	public:
		// Constructors
		program();
		bool init(string codeFile, stack<int>* b, bool strict, bool check);

		// Run Time
		int step(bool loud);
		// 0 = halt
		// 1 = success
		// 2 = breakpoint
		// 3 = error
		bool setMem(unsigned short int a, unsigned short int val);

		// Printing
		unsigned int pid();
		void print() const;
		void print(bool all) const;
		void printMem(unsigned int i) const;
		void printMem(unsigned int i, bool formated) const;

	private:
		// Variables
		unsigned short int mem[ADDR];
		doubleByte acc;
		unsigned short int counter;
		unordered_set<int> breaks;
};

// Utilities
bool checkHex(string s);

#endif
