// IBCMer - C++
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#define VERSION 0.3

#include <iostream>
#include <string>
#include <stack>

#include "src/program.h"
#include "src/doubleByte.h"
using namespace std;

// Check if string is a number
bool isNum(string s) {
	for (unsigned int i = 0; i < s.size(); i++) {
		if (!isdigit(s.at(i)))
			return false;
	}
	return true;
}
// Print help menu
void printHelp() {
	cout << "Usage: ibcmer [file] [options]" << endl;
	cout << "  --version        Print version and memory information." << endl;
	cout << "  --help, -h       Print this help menu." << endl;
	cout << "  --strict         Strictly follow format rules." << endl;
	cout << "  --check, -c      Check line numbers." << endl;
	cout << "  -b <numbers>     Sets break-points (listed in hex)." << endl;
	cout << "  --step           Starts program in debug mode." << endl;
	cout << "  --quiet          Don't print detailed output." << endl;
	cout << endl;
	cout << "Debug Environment: " << endl;
	cout << "  run                    Exit debugging and run program." << endl;
	cout << "  step [number]          Runs n commands then return to debug," << endl;
	cout << "                           defaults to running one command." << endl;
	cout << "  view [address|all]     Print contents of given memory address," << endl;
	cout << "                           all occupied memory if nothing is specified," << endl;
	cout << "                           or all memory 'all' is specified." << endl;
	cout << "  set <address> <value>  Set memory at given address to value." << endl;
	cout << "  exit                   Ends program immediately." << endl;
}

int main(int argc, char* argv[]) {
	// Check if short ints are 2 bytes
	if (sizeof(short int) != 2) {
		cerr << "PANIC: data values are the wrong size." << endl;
	}
	// Check parameters are given
	if (argc < 2) {
		cerr << "ERROR: Requires parameters." << endl;
		cerr << endl;
		printHelp();
		return 1;
	}
	// Check for stand alone parameters
	string current;
	for (int i = 1; i < argc; i++) {
		current = argv[i];
		if ((current == "--help") || (current == "-h")) {
			printHelp();
			return 0;
		}
		if (current == "--version") {
			cout << "IBCMer version " << fixed << setprecision(1) << VERSION << endl;
			return 0;
		}
	}
	// Interate through parameters
	bool strict = false;
	bool check = false;
	int step = -1;
	int loud = true;
	doubleByte temp;
	stack<int>* breaks = new stack<int>();
	for (int i = 2; i < argc; i++) {
		current = argv[i];
		if (current == "--strict") {
			strict = true;
		} else if ((current == "--check") || (current == "-c")) {
			strict = true;
			check = true;
		} else if (current == "-b") {
			if (!checkHex(argv[i+1])) {
				cerr << "ERROR: -b requires at least one break-point, break-points must be numbers." << endl;
				return 1;
			}
			do {
				i++;
				temp.setVal(argv[i]);
				breaks->push(temp.uint());
				if (i + 1 == argc)
					break;
			} while (checkHex(argv[i+1]));
		} else if (current == "--step") {
			step = 1;
		} else if ((current == "--quiet") || (current == "-q")) {
			loud = false;
		} else {
			cerr << "ERROR: Unknown parameter '" << argv[i] << "'" << endl;
			return 1;
		}
	}
	// Initialize program and memory
	string code = argv[1];
	program p = program();
	if (!p.init(code, breaks, strict, check)) {
		return 1;
	}
	// Run program
	int result = 0;
	while (true) {
		step--;
		result = p.step((loud || (step >= 0)));
		// Check for halt
		if (result == 0)
			break;
		// Check for breakpoint and go into debug mode
		if ((result == 2) || (step == 0)) {
			string input = "";
			while (true) {
				cout << "~ ";
				getline(cin, input);
				if (input == "run") {
					step = -1;
					break;
				} else if (input == "step") {
					step = 1;
					break;
				} else if (input.substr(0,4) == "step") {
					if (isNum(input.substr(5))) {
						step = atoi(input.substr(5).c_str());
						break;
					} else {
						cerr << " Step requires a number." << endl;
					}
				} else if (input == "view") {
					p.print(false);
				} else if (input == "view all") {
					p.print(true);
				} else if (input.substr(0,4) == "view") {
					if ((checkHex(input.substr(5))) && (input.size() < 9)) {
						temp = doubleByte(input.substr(5));
						p.printMem(temp.uint());
					}
				} else if (input.substr(0,3) == "set") {
					if (input.size() < 7) {
						cerr << " Set requires an address and a value." << endl;
					}
					for (unsigned int i = 1; i < input.size()-4; i++) {
						if (input.at(4+i) == ' ') {
							if ((i < 4) && (input.size()-5-i < 5) && checkHex(input.substr(4, i)) && checkHex(input.substr(5+i))) {
								doubleByte temp1 = doubleByte(input.substr(4, i));
								doubleByte temp2 = doubleByte(input.substr(5+i));
								if (!p.setMem(temp1.uint(), temp2.uint())) {
									cerr << " Invalid memory address." << endl;
								}
							} else {
								cerr << " Improperly formated address or value." << endl;
							}
							break;
						}
						if (i == input.size()-5)
							cerr << " Set requires an address and a value." << endl;
					}
				} else if (input == "exit") {
					return 0;
				} else {
					cerr << " Unknown command." << endl;
				}
			}
		}
		// Check for errors
		if (result == 3) {
			break;
		}
	}
	return 0;
}
