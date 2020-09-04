// Text - IBCMer
//   Penn Bauman
//   pennbauman@protonmail.com

// Colors
#define C_NONE "\33[0m"
#define C_BOLD "\33[1m"
#define C_RED "\33[31m"
#define C_GREEN "\33[32m"
#define C_YELLOW "\33[33m"
#define C_BLUE "\33[34m"

// Errors (with color)
#define E_ERROR "\33[31mError:\33[0m"
#define E_INVALID_IN "\33[33mInvalid Input:\33[0m"
#define E_MISSING_IN "\33[33mMissing Input\33[0m"
// Debug errors
#define E_UNKNOWN_CMD "\33[33mUnknown Command\33[0m"
#define E_INVALID_ "\33[33mInvalid"
#define E_MISSING_ "\33[33mMissing"

// Main help text
#define HELP "\33[1mIBCMer Usage\33[0m\n\
\n\
ibcmer [code-file.ibcm] [options]\n\
\n\
Options:\n\
     --version           Prints version information\n\
 -h, --help              Prints this help information\n\
 -i, --ibcm-info         Prints information about IBCM\n\
 -c, --check             Checks the code file for proper line numbers\n\
 -q, --quiet             Prevents printing of detailed program output\n\
     --silent            Prevents all printing except output (ideal for automated input)\n\
 -s, --step              Starts the program in debugging mode\n\
 -b, --break [num(s)]    Sets breakpoints to enter debugging mode, a single hex\n\
                          number or a comma separated list (no spaces)\n\
 \n\
Debug Commands:\n\
  exit                   Exits the program\n\
  run                    Exits debugging mode and runs the program normally\n\
  step (or no command)   Executes one step of the program\n\
  view all               Prints the contents of all memory containing values\n\
  view [address]         Prints the contents of a memory address (hex number)\n\
  view [addr]-[addr]     Prints the contents of the range of memory between the\n\
                          two provided addresses (both hex numbers)\n\
  set [address] [value]  Sets the value of a memory slot, both numbers must be\n\
                          provided as hexadecimal"

// IBCM information text
#define HELP_IBCM "\33[1mIBCM Information\33[0m\n\
IBCM has 4096 16 bit memory slots, a 16 bit accumulator register, and a 12 bit program counter.\n\
Code is loaded from the first 4 characters (formatted as hex) of each line of the code file.\n\
The program is executed by preforming the command in the memory slot indicated by the program\n\
counter and incrementing the counter, repeat until a halt command is reached.\n\
\n\
Command are determined by the first 4 bits (1st hex digit) of each memory slot. \n\
Later bits are used in various ways depending on command, indicated here by increasingly\n\
indented '0's, '-'s, and hex digits.\n\
\n\
\33[1m0 halt\33[0m   Exit program\n\
           000 = unused\n\
\33[1m1 i/o\33[0m    Input to or output from accumulator\n\
           0 = read hex\n\
           4 = read ASCII char\n\
           8 = print hex\n\
           C = print ASCII char\n\
             00 = unused\n\
\33[1m2 shift\33[0m  Shift accumulator bits\n\
           0 = shift left\n\
           4 = shift right\n\
           8 = rotate left\n\
           C = rotate right\n\
             0 = unused\n\
               - = distance to move\n\
\33[1m3 load\33[0m   Set accumulator to contents of address\n\
           --- = address\n\
\33[1m4 store\33[0m  Set contents of address to accumulator value\n\
           --- = address\n\
\33[1m5 add\33[0m    Add contents of memory address to accumulator\n\
           --- = address\n\
\33[1m6 sub\33[0m    Subtract contents of memory address from accumulator\n\
           --- = address\n\
\33[1m7 and\33[0m    Apply bitwise AND to contents of memory address and accumulator\n\
           --- = address\n\
\33[1m8 or\33[0m     Apply bitwise OR to contents of memory address and accumulator\n\
           --- = address\n\
\33[1m9 xor\33[0m    Apply bitwise XOR to contents of memory address and accumulator\n\
           --- = address\n\
\33[1mA not\33[0m    Apply bitwise NOT to accumulator\n\
           000 = unused\n\
\33[1mB nop\33[0m    Do nothing\n\
           000 = unused\n\
\33[1mC jmp\33[0m    Jump to address\n\
           --- = address\n\
\33[1mD jmpe\33[0m   If accumulator = 0, jump to address\n\
           --- = address\n\
\33[1mE jmpl\33[0m   If accumulator < 0, jump to address\n\
           --- = address\n\
\33[1mF brl\33[0m    Set accumulator to counter + 1 and jump to address\n\
           --- = address"
