// Text - IBCMer
//   Penn Bauman
//   pennbauman@protonmail.com

// Colors
#define C_NONE "\33[0m"
#define C_BOLD "\33[1m"
#define C_RED "\33[31m"
#define C_YELLOW "\33[33m"

// Main help text
#define HELP "\33[1mIBCMer Usage\33[0m\n\
\n\
]$ ibcmer [code-file.ibcm] [options]\n\
\n\
Options:\n\
     --version           Prints version information\n\
 -h, --help              Prints this help information\n\
 -i, --ibcm-info         Prints information about IBCM\n\
 -c, --check             Checks the code file for proper line numbers\n\
 -q, --quiet             Prevents printing of detailed program output\n\
 -s, --step              Starts the program in debugging mode\n\
 -b, --break [nums]      Sets breakpoints to enter debugging mode, a single hex\n\
                          number or a comma seperated list (no spaces)\n\
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
IBCM runs with 4096 16 bit memory slots, a 16 bit accumulator, and a 12 bit program counter.\n\
Code is loaded from the first 4 characters (formatted as hex) of each line of the code file.\n\
The program is executed by preforming the command in the memory slot indicated by the program\n\
counter and incrementing the counter, repeat until a halt command is reached.\n\
\n\
Command are determined by the first 4 bits (1st hex digit) of each memory slot. \n\
Later bits are used in various ways depending on command, indicated here by incressingly\n\
indented '0's, '-'s, and hex digits.\n\
\n\
\33[1m0 halt\33[0m   exit program\n\
           000 = unused\n\
\33[1m1 i/o\33[0m    input to or output from accumulator\n\
           0 = read hex\n\
           4 = read ascii char\n\
           8 = print hex\n\
           C = print ascii char\n\
             00 = unused\n\
\33[1m2 shift\33[0m  shift accumulator bits\n\
           0 = shift left\n\
           4 = shift right\n\
           8 = rotate left\n\
           C = rotate right\n\
             0 = unused\n\
               - = distance to move\n\
\33[1m3 load\33[0m   set accumulator to contents of address\n\
           --- = address\n\
\33[1m4 store\33[0m  set contents of address to accumulator value\n\
           --- = address\n\
\33[1m5 add\33[0m    add contents of memory address to accumulator\n\
           --- = address\n\
\33[1m6 sub\33[0m    subtract contents of memory address from accumulator\n\
           --- = address\n\
\33[1m7 and\33[0m    apply bitwise and to contents of memory address and accumulator\n\
           --- = address\n\
\33[1m8 or\33[0m     apply bitwise or to contents of memory address and accumulator\n\
           --- = address\n\
\33[1m9 xor\33[0m    apply bitwise xor to contents of memory address and accumulator\n\
           --- = address\n\
\33[1mA not\33[0m    apply bitwise not to accumulator\n\
           000 = unused\n\
\33[1mB nop\33[0m    do nothing\n\
           000 = unused\n\
\33[1mC jmp\33[0m    jump to address\n\
           --- = address\n\
\33[1mD jmpe\33[0m   if ACC = 0, jump to address\n\
           --- = address\n\
\33[1mE jmpl\33[0m   if ACC < 0, jump to address\n\
           --- = address\n\
\33[1mF brl\33[0m    set accumulator to counter + 1 and jump to address\n\
           --- = address"
