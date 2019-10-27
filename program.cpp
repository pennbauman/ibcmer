/* Penn Bauman
 * pennbauman@protonmail.com
 */
#include "program.h"
using namespace std;

program::program() {
	//memInit();
	for (int i = 0; i < ADDR; i++) {
		mem[i] = 0;
	}
	counter = 0;
}
program::~program() {

}


bool program::init(string codeFile, stack<int>* b, bool strict, bool check) {
	string line;
	ifstream file(codeFile.c_str());
	int i = 0;
	doubleByte temp;
	if (file.is_open()) {
		while (getline(file,line)) {
			if (i == ADDR) {
				cerr << "instruction overflow" << endl;
				return false;
			}
			if (checkHex(line.substr(0,4))) {
				temp = doubleByte(line.substr(0,4));
				mem[i] = temp.uint();
				if (check) {
					//cout << line.substr(5,3) << endl;
					if (checkHex(line.substr(5,3))) {
						temp = doubleByte("0" + line.substr(5,3));
						if (temp.uint() != i) {
							cout << "Improper line number at line " << i << endl;
							return false;
						}
					} else {
						cout << "Improper line number at line " << i << endl;
						return false;
					}
				}
			} else {
				if (((line == "") || (line.substr(0,4) == "    ")) && !strict) {
					cout << "_" << endl;
				} else {
					cerr << "improper instructions at " << i << " : " << line << endl;
					cerr << line.substr(0,4) << checkHex(line.substr(0,4)) << endl;
					return false;
				}
			}
			i++;
		}
	} else {
		cerr << ".ibcm file not opened" << endl;
		return false;
	}
	file.close();
	while (!b->empty()) {
		breaks.push(b->top());
		b->pop();
	}
	return true;
}
int program::step() {
	doubleByte command = doubleByte(mem[counter]);
	cout << hex << "[" << setfill('0') << setw(3) << counter << "]" << setw(4) << mem[counter] << "  ";
	//command.print();
	unsigned short int result = 0;
	switch(command.cat(0)) {
		case '0': // halt
			cout << "halt" << endl;
			return 0;
		case '1': // I/O
			cout << "I/O   ";
			if (command.bat(4)) {
				cout << "(ACC)" << acc.str() << endl;
				cout << "Output: ";
				if (command.bat(5)) {
					cout << acc.ascii() << endl;
				} else {
					cout << acc.str() << endl;
				}
			} else {
				//cout << "input" << endl;
				string in;
				cout << endl;
				if (command.bat(5)) {
					cout << "Input ASCII char: ";
					getline(cin, in);
					if (in.size() == 1) {
						unsigned short int a = in.at(0);
						acc = doubleByte(a);
					} else {
						cout << "improper ascii char" << endl;
						return 3;
					}
				} else {
					cout << "Input 4-digit hex: ";
					getline(cin, in);
					if ((checkHex(in)) && (in.size() <= 4)) {
						acc = doubleByte(in);
					} else {
						cerr << "unreadable hex word" << endl;
						return 3;
					}
				}
			}
			break;
		case '2': // shift
			cout << "shift (ACC)";
			for (int i = 12; i < 16; i++) {
				if (command.bat(i))
					result += pow(2, (15-i));
			}
			if (command.bat(4)) {
				acc.shiftRight(result ,command.bat(5));
			} else {
				acc.shiftLeft(result ,command.bat(5));
			}
			cout << acc.str() << endl;
			break;
		case '3': // load
			cout << "load  [" << hex << setfill('0') << setw(3) << command.addr() << "]";
			acc = doubleByte(mem[command.addr()]);
			cout << acc.str() << endl;
			break;
		case '4': // store
			cout << "store [" << hex << setfill('0') << setw(3) << command.addr() << "]" << acc.str() << endl;
			mem[command.addr()] = acc.uint();
			break;
		case '5': // add
			cout << "add   " << "(ACC)" << acc.str() << " - [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			result = mem[command.addr()];
			result = acc.add(result);
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case '6': // sub
			cout << "sub   " << "(ACC)" << acc.str() << " - [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			result = mem[command.addr()];
			result = acc.sub(result);
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case '7': // and
			cout << "and   " << "(ACC)" << acc.str() << " & [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			result = mem[command.addr()];
			result = acc.band(result);
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case '8': // or
			cout << "or    " << "(ACC)" << acc.str() << " | [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			result = mem[command.addr()];
			result = acc.bor(result);
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case '9': // xor
			cout << "xor   " << "(ACC)" << acc.str() << " ^ [" << hex << setfill('0') << setw(3) << command.addr() << "]" << setw(4) << mem[command.addr()];
			result = mem[command.addr()];
			result = acc.bxor(result);
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case 'a': // not
			cout << "not" << endl;
			cout << "not   " << "(ACC)" << acc.str() << " !";
			result = acc.bnot();
			cout << " = " << setw(4) << result << endl;
			acc = doubleByte(result);
			break;
		case 'b': // nop
			cout << "nop" << endl;
			break;
		case 'c': // jmp
			cout << "jmp   [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
			counter = command.addr() - 1;
			break;
		case 'd': // jmpe
			if (acc.sint() == 0) {
				cout << "jmpe  [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
				counter = command.addr() - 1;
			} else {
				cout << "jmpe  (ACC)" << acc.str() << endl;
			}
			break;
		case 'e': // jmpl
			if (acc.sint() < 0) {
				cout << "jmpl  [" << hex << setfill('0') << setw(3) << command.addr() << "]" << endl;
				counter = command.addr() - 1;
			} else {
				cout << "jmpl  (ACC)" << acc.str() << endl;
			}
			break;
		case 'f': // brl
			cout << "brl   [" << hex << setfill('0') << setw(3) << command.addr() << "]  (ACC)";
			counter++;
			acc = doubleByte(counter);
			cout << acc.str() << endl;
			counter = command.addr() - 1;
			break;
		default:
			return 3;
	}
	counter++;
	if (breaks.top() == counter) {
		breaks.pop();
		return 2;
	}
	return 1;
}

unsigned int program::pid() {
	return counter;
}

void program::print() const {
	cout << "PID: " << hex << setfill('0') << setw(3) << counter << "  ACC: " << setw(4) << acc.uint() << endl;
	printMem();
}
//void program::printPID() const {}
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

bool checkHex(string s) {
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
				//cout << s.at(i) << endl;
				return false;
		}
	}
	return true;
}
