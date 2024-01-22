#pragma once

#ifndef _PUSH_POP_H
# define _PUSH_POP_H

# define REG_RAX 0
# define REG_RBX 1
# define REG_RCX 2
# define REG_RDX 3

# define CMD_STATUS "status"
# define CMD_PUSH "push"
# define CMD_POP "pop"

# define DEF_STACK_BOT 0x140
# define DEF_STACK_LIMIT 0x100
# define DEF_RSP 0x130

# define REG_SIZE (sizeof(long))
# define WHITESPACE " \t"

// i: name of input file
// o: name of output file
// b: stack_bottom (high address, as hex)
// l: stack limit (low address, as hex)
// s: beginning rsp value (within the stack, as hex)
// v  verbose output
//      all diagnostic output must got to stderr
// h  help
# define GETOPT_STRING "i:o:b:l:s:vh"
# define BUFFER_SIZE 100

void stack_status(unsigned long *, unsigned long *, unsigned long
                  , unsigned long, unsigned long);

#endif // _PUSH_POP_H
