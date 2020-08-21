// Text - IBCMer
//   Penn Bauman
//   pennbauman@protonmail.com

#define C_NONE "\33[0m"
#define C_BOLD "\33[1m"
#define C_ITALIC "\33[3m"
#define C_RED "\33[31m"
#define C_YELLOW "\33[33m"

#define HELP "\33[1mIBCMer Usage\33[0m\n\
\n\
 $ ibcmer [code-file.ibcm] [options]\n\
\n\
Options:\n\
     --version    Prints version information\n\
 -h, --help       Prints this help information\n\
     --help-ibcm  Prints information about IBCM\n\
 -c, --check      Checks the code file for proper line numbers\n\
 -q, --quiet      Prevents printing of detailed program output during execution\n\
 -s, --step       Starts the program in debugging mode\n\
 -b, --break [n]  Sets breakpoints to enter debugging mode, formatted as a single\n\
                   hexadecimal number of a comma seperated list (no spaces)\n\
 \n\
Debug Commands:\n\
  exit                  Exits the program\n\
  run                   Exits debugging mode and runs the program normally\n\
  step                  Executes one step of the program\n\
  view [address]        Print the contents of memory. If a single hexadecimal\n\
                         address is provided that memory slot is printed, If two\n\
                         address are provided separated by a '-' the range between\n\
                         them is printed, If 'all' is provided then the range of\n\
                         memory containing values is printed\n\
  set [address] [value] Sets the value of a memory slot, both numbers must be\n\
                         provided as hexadecimal"

#define HELP_IBCM "\33[1mIBCM Information\33[0m\n\
IBCM runs with 4096 16 bit memory slots, a 16 bit accumulator, and a 12 bit program counter. Code is loaded from the first 4 characters (formatted as hex) of each line of the code file. The program is executed by preforming the command in the memory slot indicated by the program counter and incrementing the counter, repeat until a halt command is reached.\n\
\n\
Commands types are determined by the first 4 bits (1st hex digit) of each memory slot. Later bits are used in various ways depending on command type (indicated here by indented '0's and '#'s.\n\
\n\
0 halt   exit program\n\
      000 = unused\n\
\n\
1 i/o    input to or output from accumulator\n\
      0 = read hex\n\
      4 = read ascii char\n\
      8 = print hex\n\
      C = print ascii char\n\
        00 = unused\n\
\n\
2 shift  shift accumulator bits\n\
      0 = shift left\n\
      4 = shift right\n\
      8 = rotate left\n\
      C = rotate right\n\
        0 = unused\n\
          # = distance to move\n\
\n\
3 load   set accumulator to contents of address\n\
      ### = address\n\
\n\
4 store  set contents of address to accumulator value\n\
      ### = address\n\
\n\
5 add    add contents of memory address to accumulator\n\
      ### = address\n\
\n\
6 sub    subtract contents of memory address from accumulator\n\
      ### = address\n\
\n\
7 and    apply bitwise and to contents of memory address and accumulator\n\
      ### = address\n\
\n\
8 or     apply bitwise or to contents of memory address and accumulator\n\
      ### = address\n\
\n\
9 xor    apply bitwise xor to contents of memory address and accumulator\n\
          ### = address\n\
\n\
A not    apply bitwise not to accumulator\n\
          000 = unused\n\
\n\
B nop    do nothing\n\
          000 = unused\n\
\n\
C jmp    jump to address\n\
          ### = address\n\
\n\
D jmpe   if ACC = 0, jump to address\n\
          ### = address\n\
\n\
E jmpl   if ACC < 0, jump to address\n\
          ### = address\n\
\n\
F brl    set accumulator to counter + 1 and jump to address\n\
          ### = address"
