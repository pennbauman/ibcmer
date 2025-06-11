// Program Class - IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#include "program.h"
using namespace std;

// Constructors
program::program() {
	for (int i = 0; i < ADDR; i++) {
		mem[i] = 0;
	}
	counter = 0;
	breaks = unordered_set<int>(7);
}
bool program::init(string codeFile, stack<int>* b, bool strict, bool check) {
	// Load memory from code file
	string line;
	ifstream file(codeFile.c_str());
	int i = 0;
	doubleByte temp;
	if (file.is_open()) {
		while (getline(file, line)) {
			if (i == ADDR) {
				cerr << "Error: Code file overflows memory (" << ADDR << " lines max)" << endl;
				return false;
			}
			if ((line.length() >= 4) && (checkHex(line.substr(0,4)))) {
				// Set memory
				temp.setVal(line.substr(0,4));
				mem[i] = temp.uint();
				if (check) {
					// Check line number if appropriate
					if (checkHex(line.substr(5,3))) {
						temp.setVal(line.substr(5,3));
						if (temp.uint() != i) {
							cerr << "Error: Improper line number at line " << i << endl;
							return false;
						}
					} else {
						cerr << "Error: Improper line number at line " << i << endl;
						return false;
					}
				}
			} else {
				// Check if line can be ignored
				if (!((line.size() == 0) || (line.substr(0,4) == "    ")) || strict) {
					int j = 0;
					while (std::isxdigit(line[j])) {
						j++;
					}
					cerr << "Error: '" << codeFile << ":" << (i + 1) << ":" << (j + 1) << "' Invalid opcode hexadecimal" << endl;
					cerr << endl << "    " << line << endl << "    ";
					for (int k = 0; k < j; k++) {
						cerr << " ";
					}
					cerr << "^" << endl;
					return false;
				} else {
					i--;
				}
			}
			i++;
		}
	} else {
		cerr << "Error: Code file '" << codeFile << "' not found" << endl;
		return false;
	}
	file.close();
	// Load breakpoints from stack
	while (!b->empty()) {
		breaks.insert(b->top());
		b->pop();
	}
	return true;
}

// Run Time
int program::step(bool loud) {
	if (counter == ADDR) {
		cerr << "Error: Memory overflow (PC = 0x" << hex << setfill('0') << setw(3) << counter << ")"<< endl;
		return 3;
	}
	// Load command
	doubleByte command = doubleByte(mem[counter]);
	string prev;
	if (loud)
		cout << hex << "[" << setfill('0') << setw(3) << counter << "]" << command.str() << "  ";
	unsigned short int result = 0;
	switch(command.cat(0)) {
		case '0': // halt
			if (loud)
				cout << "halt" << endl;
			return 0;
		case '1': // I/O
			if (loud)
				cout << "i/o   (ACC)" << acc.str() << endl;
			if (command.bat(6) || command.bat(7)) {
				cout << endl;
				cerr << "Error: Invalid I/O sub-opcode '" << command.cat(1) << "'" << endl;
				return 3;
			}
			if (command.bat(4)) {
				if (command.bat(5)) {
					cout << "Output char: " << acc.ascii() << endl;
				} else {
					cout << "Output hex:  " << acc.str() << endl;
				}
			} else {
				string in;
				while (true) {
					if (command.bat(5)) {
						cout << "Input char: ";
						getline(cin, in);
						if (in.size() == 1) {
							unsigned short int a = in.at(0);
							acc.setVal(a);
							break;
						} else {
							cerr << "  Improper ASCII character." << endl;
						}
					} else {
						cout << "Input hex:  ";
						getline(cin, in);
						if ((checkHex(in)) && (in.size() <= 4)) {
							acc.setVal(in);
							break;
						} else {
							cerr << "  Unreadable hex word." << endl;
						}
					}
				}
			}
			break;
		case '2': // shift
			prev = acc.str();
			for (int i = 12; i < 16; i++) {
				if (command.bat(i))
					result += pow(2, (15-i));
			}
			if (command.bat(5)) {
				acc.shiftRight(result, command.bat(4));
			} else {
				acc.shiftLeft(result, command.bat(4));
			}
			if (command.bat(6) || command.bat(7)) {
				cout << "shift " << endl;
				cerr << "Error: Invalid shift sub-opcode '" << command.cat(1) << "'" << endl;
				return 3;
			}
			if (loud) {
				cout << "shift (ACC)" << acc.str() << " = (ACC)" << prev ;
				if (command.bat(5)) {
					if (command.bat(4)) {
						cout << " => " << hex << result << endl;
					} else {
						cout << " >> " << hex << result << endl;
					}
				} else {
					if (command.bat(4)) {
						cout << " <= " << hex << result << endl;
					} else {
						cout << " << " << hex << result << endl;
					}
				}
			}
			break;
		case '3': // load
			acc.setVal(mem[command.addr()]);
			if (loud)
				cout << "load  (ACC)" << acc.str() << " = [" << hex << setfill('0') << setw(3) << command.addr() << "]" << acc.str() << endl;
			break;
		case '4': // store
			if (loud)
				cout << "store [" << hex << setfill('0') << setw(3) << command.addr() << "]" << acc.str() << " = (ACC)" << acc.str() << endl;
			mem[command.addr()] = acc.uint();
			break;
		case '5': // add
			prev = acc.str();
			acc.add(mem[command.addr()]);
			if (loud)
				cout << "add   " << "(ACC)" << acc.str() << " = (ACC)" << prev << " + [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()] << endl;
			break;
		case '6': // sub
			prev = acc.str();
			acc.sub(mem[command.addr()]);
			if (loud)
				cout << "sub   " << "(ACC)" << acc.str() << " = (ACC)" << prev << " - [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()] << endl;
			break;
		case '7': // and
			prev = acc.str();
			acc.band(mem[command.addr()]);
			if (loud)
				cout << "and   " << "(ACC)" << acc.str() << " = (ACC)" << prev << " & [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()] << endl;
			break;
		case '8': // or
			prev = acc.str();
			acc.bor(mem[command.addr()]);
			if (loud)
				cout << "or    " << "(ACC)" << acc.str() << " = (ACC)" << prev << " | [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()] << endl;
			break;
		case '9': // xor
			prev = acc.str();
			acc.bxor(mem[command.addr()]);
			if (loud)
				cout << "xor   " << "(ACC)" << acc.str() << " = (ACC)" << prev << " ^ [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()] << endl;
			break;
		case 'a': // not
			prev = acc.str();
			acc.bnot();
			if (loud)
				cout << "not   " << "(ACC)" << acc.str() << " = ! (ACC)" << prev << endl;
			break;
		case 'b': // nop
			if (loud)
				cout << "nop" << endl;
			break;
		case 'c': // jmp
			if (loud)
				cout << "jmp   [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
			counter = command.addr() - 1;
			break;
		case 'd': // jmpe
			if (acc.sint() == 0) {
				if (loud)
					cout << "jmpe  [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
				counter = command.addr() - 1;
			} else {
				if (loud)
					cout << "jmpe  (ACC)" << acc.str() << endl;
			}
			break;
		case 'e': // jmpl
			if (acc.sint() < 0) {
				if (loud)
					cout << "jmpl  [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
				counter = command.addr() - 1;
			} else {
				if (loud)
					cout << "jmpl  (ACC)" << acc.str() << endl;
			}
			break;
		case 'f': // brl
			if (loud)
				cout << "brl   [" << hex << setfill('0') << setw(3) << command.addr() << "]  (ACC)";
			counter++;
			acc.setVal(counter);
			if (loud)
				cout << acc.str() << endl;
			counter = command.addr() - 1;
			break;
		default: // unknown op code
			cerr << "Error: Invalid opcode" << endl;
			return 3;
	}
	counter++;
	// Check break-point
	if (breaks.count(counter - 1) == 1) {
		return 2;
	}
	return 1;
}
bool program::setMem(unsigned short int a, unsigned short int val) {
	if (a >= ADDR)
		return false;
	mem[a] = val;
	return true;
}

// Printing
unsigned int program::pid() {
	return counter;
}
void program::print() const {
	cout << "PID: " << hex << setfill('0') << setw(3) << counter << "  ACC: " << setw(4) << acc.uint() << endl;
	print(true);
}
void program::print(bool all) const {
	bool zero = false;
	for (int i = 0; i < ADDR; i++) {
		printMem(i, true);
		if (((i+1)%4 == 0) && !all) {
			zero = true;
			for (int j = i+1; j < ADDR; j++) {
				if (mem[j] != 0) {
					zero = false;
					break;
				}
			}
			if (zero)
				break;
		}
	}
}
void program::printMem(unsigned int i) const {
	printMem(i, false);
}
void program::printMem(unsigned int i, bool formated) const {
	if (i >= ADDR)
		return;
	cout << "[" << setfill('0') << setw(3) << hex << i << "]" << setfill('0') << setw(4) << hex << mem[i];
	if (formated) {
	   cout << "    ";
		if ((i+1)%4 == 0)
			cout << endl;
	} else {
		cout << endl;
	}
}

// Utilities
bool checkHex(string s) {
	if (s.size() == 0)
		return false;
	for (unsigned int i = 0; i < s.size(); i++) {
		switch(s.at(i)) {
			case '0': break;
			case '1': break;
			case '2': break;
			case '3': break;
			case '4': break;
			case '5': break;
			case '6': break;
			case '7': break;
			case '8': break;
			case '9': break;
			case 'a': break;
			case 'b': break;
			case 'c': break;
			case 'd': break;
			case 'e': break;
			case 'f': break;
			case 'A': break;
			case 'B': break;
			case 'C': break;
			case 'D': break;
			case 'E': break;
			case 'F': break;
			default:
				return false;
		}
	}
	return true;
}
