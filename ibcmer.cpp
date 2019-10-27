/* Penn Bauman
 * pennbauman@protonmail.com
 */
#define VERSION 0.1

#include <iostream>
#include <string>
#include <stack>

#include "program.h"
#include "sortStack.h"
#include "doubleByte.h"
using namespace std;

bool isNum(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (!isdigit(s.at(i)))
			return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (sizeof(short int) != 2) {
		cerr << "PANIC" << endl;
	}
	if (argc < 2) {
		cerr << "requires parameters, set --help" << endl;
		return 1;
	}

	string current;
	for (int i = 1; i < argc; i++) {
		current = argv[i];
		if ((current == "--help") || (current == "-h")) {
			cout << "Usage: ibcmer [file] [options]" << endl;
			cout << "  --version       Print version and memory info." << endl;
			cout << "  -h, --help      Print this help menu." << endl;
			cout << "  --strict        Strictly follow format rules." << endl;
			cout << "  -c, --check     Check line numbers." << endl;
			cout << "  -b <numbers>    Sets break-points." << endl;
			cout << "  --step          Starts program in debug mode." << endl;
			cout << "  --quiet         Don't print detailed output." << endl;
			cout << endl;
			cout << "Debug Enviorment: " << endl;
			cout << "  run             Exit debugging and run program." << endl;
			cout << "  step [number]   Runs n commands then return to debug," << endl;
			cout << "                   defaults to running one command." << endl;
			cout << "  view [address]  Print contents of memory address," << endl;
			cout << "                   or all memory if none specified." << endl;
			return 0;
		}
		if (current == "--version") {
			cout << "IBCMer version " << fixed << setprecision(1) << VERSION << " with " << ADDR << " memory slots." << endl;
			return 0;
		}
	}
	bool strict = false;
	bool check = false;
	int step = -1;
	int loud = true;
	stack<int>* breaks = new stack<int>();
	for (int i = 2; i < argc; i++) {
		current = argv[i];
		if (current == "--strict") {
			strict = true;
		} else if ((current == "--check") || (current == "-c")) {
			strict = true;
			check = true;
		} else if (current == "-b") {
			if (!isNum(argv[i+1])) {
				cerr << "-b requires at least one break-point, break-points must be numbers." << endl;
				cout << argv[i+1] << endl;
				return 1;
			}
			do {
				i++;
				breaks->push(atoi(argv[i]));
				if (i + 1 == argc)
					break;
			} while (isNum(argv[i+1]));
		} else if (current == "--step") {
			step = 1;
		} else if ((current == "--quiet") || (current == "-q")) {
			loud = false;
		} else {
			cerr << "unknown parameter: " << argv[i] << endl;
			return 1;
		}
	}
	string code = argv[1];
	program p = program();
	if (!p.init(code, breaks, strict, check)) {
		return 1;
	}
	int result = 0;
	while (true) {
		step--;
		result = p.step((loud || (step >= 0)));
		if (result == 0)
			break;
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
						cout << "step requires a number." << endl;
					}
				} else if (input == "view") {
					p.print();
				} else if (input.substr(0,4) == "view") {
					if ((checkHex(input.substr(5))) && (input.size() < 9)) {
						doubleByte temp = doubleByte(input.substr(5));
						p.printMem(temp.uint());
					}
				} else {
					cout << "Unknown command." << endl;
				}
			}
		}
		if (result == 3) {
			cout << "error?" << endl;
			break;
		}
	}
	return 0;
}
