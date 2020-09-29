// IBCMer - Debugging Tools
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdio.h>

#include "text.h" // Defines text variables
#include "executor.h" // IBCM executor

// Breakpoints list
typedef struct breakpoints {
	unsigned short *array; // Pointer to breakpoints array
	unsigned short count; // Length of breakpoints list
	unsigned short max; // Length if breakpoints array
} breakpoints;

// Initialize breakpoints
breakpoints init_breakpoints();
// Check if breakpoint exists
signed char is_breakpoint(breakpoints *breaks, unsigned short num);
// Add new breakpoint
void add_breakpoint(breakpoints *breaks, unsigned short num);

// Run debug commands
unsigned char debug(ibcmemory *data, breakpoints *breaks, char *cmd, unsigned char volume);

#endif
