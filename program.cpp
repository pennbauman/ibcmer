/* Penn Bauman
 * pennbauman@protonmail.com
 */
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
		while (getline(file,line)) {
			if (i == ADDR) {
				cerr << "ERROR: Instructions overflow memory." << endl;
				return false;
			}
			if (checkHex(line.substr(0,4))) {
				// Set memory
				temp.setVal(line.substr(0,4));
				mem[i] = temp.uint();
				if (check) {
					// Check line number if appropriate
					if (checkHex(line.substr(5,3))) {
						temp.setVal(line.substr(5,3));
						if (temp.uint() != i) {
							cerr << "ERROR: Improper line number at line " << i << "." << endl;
							return false;
						}
					} else {
						cerr << "ERROR: Improper line number at line " << i << "." << endl;
						return false;
					}
				}
			} else {
				// Check if line can be ignored
				if (!((line == "") || (line.substr(0,4) == "    ")) || strict) {
					cerr << "ERROR: Improper instructions at " << hex << i << " '" << line.substr(0,4) << "'" << endl;
					return false;
				}
			}
			i++;
		}
	} else {
		cerr << "ERROR: " << codeFile << " not openable." << endl;
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

// Run
int program::step(bool loud) {
	if (counter == ADDR) {
		cerr << "ERROR: Program overran memory." << endl;
		return 3;
	}
	// Load command
	doubleByte command = doubleByte(mem[counter]);
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
				cout << "i/o   ";
			if (command.bat(4)) {
				if (loud)
					cout << "(ACC)" << acc.str() << endl;
				cout << "Output: ";
				if (command.bat(5)) {
					cout << acc.ascii() << endl;
				} else {
					cout << acc.str() << endl;
				}
			} else {
				string in;
				if (loud)
					cout << endl;
				while (true) {
					if (command.bat(5)) {
						cout << "Input ASCII char: ";
						getline(cin, in);
						if (in.size() == 1) {
							unsigned short int a = in.at(0);
							acc.setVal(a);
							break;
						} else {
							cerr << "  Improper ASCII character." << endl;
						}
					} else {
						cout << "Input 4-digit hex: ";
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
			if (loud)
				cout << "shift (ACC)" << acc.str() << " -> (ACC)";
			for (int i = 12; i < 16; i++) {
				if (command.bat(i))
					result += pow(2, (15-i));
			}
			if (command.bat(4)) {
				acc.shiftRight(result ,command.bat(5));
			} else {
				acc.shiftLeft(result ,command.bat(5));
			}
			if (loud)
				cout << acc.str() << endl;
			break;
		case '3': // load
			if (loud)
				cout << "load  [" << hex << setfill('0') << setw(3) << command.addr() << "]";
			acc.setVal(mem[command.addr()]);
			if (loud)
				cout << acc.str() << endl;
			break;
		case '4': // store
			if (loud)
				cout << "store [" << hex << setfill('0') << setw(3) << command.addr() << "]" << acc.str() << endl;
			mem[command.addr()] = acc.uint();
			break;
		case '5': // add
			if (loud)
				cout << "add   " << "(ACC)" << acc.str() << " + [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			acc.add(mem[command.addr()]);
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
			break;
		case '6': // sub
			if (loud)
				cout << "sub   " << "(ACC)" << acc.str() << " - [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			acc.sub(mem[command.addr()]);
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
			break;
		case '7': // and
			if (loud)
				cout << "and   " << "(ACC)" << acc.str() << " & [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			acc.band(mem[command.addr()]);
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
			break;
		case '8': // or
			if (loud)
				cout << "or    " << "(ACC)" << acc.str() << " | [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			acc.bor(mem[command.addr()]);
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
			break;
		case '9': // xor
			if (loud)
				cout << "xor   " << "(ACC)" << acc.str() << " ^ [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			acc.bxor(mem[command.addr()]);
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
			break;
		case 'a': // not
			if (loud)
				cout << "not   " << "(ACC)" << acc.str() << " !";
			acc.bnot();
			if (loud)
				cout << " = " << setw(4) << acc.str() << endl;
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
			cerr << "ERROR: Unknown operation code." << endl;
			return 3;
	}
	counter++;
	// Check break-point
	if (breaks.count(counter) == 1) {
		return 2;
	}
	return 1;
}

// Printing
unsigned int program::pid() {
	return counter;
}
void program::print() const {
	cout << "PID: " << hex << setfill('0') << setw(3) << counter << "  ACC: " << setw(4) << acc.uint() << endl;
	printMem();
}
void program::printMem() const {
	for (int i = 0; i < ADDR; i++) {
		printMem(i, true);
	}
}
void program::printMem(unsigned int i) const {
	printMem(i, false);
}
void program::printMem(unsigned int i, bool formated) const {
	if (i >= ADDR)
		return;
	cout << "[" << setfill('0') << setw(3) << hex << i << "] " << setfill('0') << setw(4) << hex << mem[i];
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
	for (int i = 0; i < s.size(); i++) {
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
