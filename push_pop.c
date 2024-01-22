#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "push_pop.h"

#define MAX 100

// The valid commands:
//   status
//   push %reg
//   push $val
//   pop %reg

int pushpop_flag = 1;
int is_verbose = 0;
FILE *ofile = NULL;
static unsigned long stack_bot = DEF_STACK_BOT; // the high address for the stack
static unsigned long stack_limit = DEF_STACK_LIMIT; // the low address for the stack
static unsigned long rsp = DEF_RSP; // the initial value for the rsp register
static unsigned long * stack = NULL; // points to the bytes for the stack
static unsigned long registers[REG_RDX + 1] = { // the general purpose registers
    REG_RAX + 20
    , REG_RBX + 30
    , REG_RCX + 40
    , REG_RDX + 50
};

static void pop(char *, unsigned long *, unsigned long *);
static void push_reg(char *, unsigned long *, unsigned long *);
static void push_value(char *, unsigned long*);

static void
pop(char *reg, unsigned long * regs, unsigned long * sptr)
{
	// pop will store what the value rsp is holding into the given register
	unsigned long value = 0;

	value = *sptr;

	switch (reg[2]) // reg is the string here. Since registers follow the %xyx where x can be any letter and y is either a,b,c, or d, I want to check the y in this case.
	{
		case 'a':
			regs[0] = value;
			break;
		case 'b':
			regs[1] = value;
			break;
		case 'c':
			regs[2] = value;
			break;
		case 'd':
			regs[3] = value;
			break;
		default:
			printf("unrecognized operand for pop: %s\n", reg);
			pushpop_flag = 0;
			break;
	}
}

static void
push_reg(char *reg, unsigned long * regs, unsigned long * sptr)
{
	unsigned long value = 0;
	int flag = 1;

	switch (reg[2])
	{
		case 'a':
			value = regs[0];
			break;
		case 'b':
			value = regs[1];
			break;
		case 'c':
			value = regs[2];
			break;
		case 'd':
			value = regs[3];
			break;
		default:
			printf("unrecognized register: %s\n", reg);
			flag = 0;	
			pushpop_flag = 0;
			break;
	}

	if (flag)
		*sptr = value; // set the stack pointer 
}

static void
push_value(char *reg, unsigned long * sptr)
{
	unsigned long value = 0;
	//char * token;

	//token = strtok(reg, "$");

	sscanf(reg, "%lx", &value);

	*sptr = value;
}

int main(int argc, char *argv[])
{
    FILE *ifile = stdin;
    char * ifile_name = NULL;
    char * ofile_name = NULL;
    char input[MAX] = {0};
    char * token = NULL;
    unsigned long hex_bottom = stack_bot; // hex's bottom (aka not the top of the stack or high address)
    unsigned long hex_lowadd = stack_limit; // hex's low address (aka the top of the stack)
    unsigned long stackptr = rsp; // stack pointer.
    unsigned long * stackarr = stack; // will need to dynamically allocate this array based on the default size or user specified size.
//    unsigned long regs[REG_RDX + 1] = 

    int arrsize = 9; // for the allocated array
    unsigned long * arrptr = NULL;
    //int len = 0;
    ofile = stdout;


    
    {
        int opt = 0;
        //int res_count = 0;

        while ((opt = getopt(argc, argv, GETOPT_STRING)) != -1) { // this getopt will handle the command options
            switch (opt) {

//error stuff goes in here
		    case 'i':
			    ifile_name = optarg;
			    ifile = fopen(ifile_name, "r");
			    if (ifile == NULL)
			    {
				    perror("Failed to open the file\n");
				    exit(EXIT_FAILURE);
			    }
			    break;

		    case 'o':
			    ofile_name = optarg;
			    ofile = fopen(ofile_name, "w");
			    break;

		    case 'b':
			    sscanf(optarg, "%lx", &hex_bottom);

			    if ((hex_bottom % 8) != 0) // checking if multiple of 8.
			    {
				    printf("stack_bot limit must be multiple of 8\n");
				    exit(EXIT_FAILURE);
			    }
			    /*
			    if (hex_bottom < hex_lowadd)
			    {
				    printf("stack botton must be higher than stack limit\n");
				    exit(EXIT_FAILURE);
			    }
			    */
			    break;

		    case 'l':
			    sscanf(optarg, "%lx", &hex_lowadd);

			    if ((hex_lowadd % 8) != 0) // checking if multiple of 8.
			    {
				    printf("stack_limit value must be multiple of 8\n");
				    exit(EXIT_FAILURE);
			    }

			    // check if low address is more than the high address
			    /*
			    if (hex_lowadd > hex_bottom)
			    {
				    printf("stack botton must be higher than stack limit\n");
				    exit(EXIT_FAILURE);
			    }
			    */
			    break;

		    case 's':
			    sscanf(optarg, "%lx", &stackptr);

			    if ((stackptr % 8) != 0)
			    {
				    printf("initial rsp value must be multiple of 8\n");
				    exit(EXIT_FAILURE);
			    }
			    /*

			    if (stackptr < hex_lowadd || stackptr > hex_bottom)
			    {
				    printf("initial rsp value outside stack");
				    exit(EXIT_FAILURE);
			    }
			    */

			    break;

		    case 'v':
			    is_verbose = 1;
			    break;

		    case 'h':
			
			printf("%s\n\toptions: %s\n", argv[0], GETOPT_STRING);
			printf("\ti: name of input file\n");
			printf("\to: name of output file\n");
			printf("\tb: stack_bottom (high address, as hex)\n");
			printf("\tl: stack limit (low address, as hex)\n");
			printf("\ts: beginning %%rsp value (within the stack, as hex)\n");
			printf("\tv  verbose output\n");
			printf("\th  help\n");

			exit(EXIT_SUCCESS);
			break;

		    default:
			break;
            }
        }

	if (hex_lowadd > hex_bottom)
	    {
		    printf("stack botton must be higher than stack limit\n");
		    exit(EXIT_FAILURE);
	    }
	if (hex_bottom < hex_lowadd)
	    {
		    printf("stack botton must be higher than stack limit\n");
		    exit(EXIT_FAILURE);
	    }
	if (stackptr < hex_lowadd || stackptr > hex_bottom)
	    {
		    printf("initial rsp value outside stack");
		    exit(EXIT_FAILURE);
	    }


	// Getting the array size using some math
	arrsize = ((hex_bottom - hex_lowadd) / 8 ) + 1;
	//printf("ARRSIZE = %d\n", arrsize);

	if (is_verbose == 1)
	{
		fprintf(stderr, ">> allocating stack: %lu bytes %d rows\n", arrsize * sizeof(unsigned long), arrsize);
	}

	stackarr = (unsigned long *) calloc(arrsize, sizeof(unsigned long *));

	arrptr = &stackarr[(hex_bottom - stackptr) / 8]; //arrptr is the stackarr pointer.

	//printf("STACK BOTTOM or HIGH ADDRESS: %ld. STACK POINTER: %ld.\n", hex_bottom, stackptr);
	//printf("ARRAY STACK POINTER AT = %ld\n", (hex_bottom - stackptr) / 8);

	// Check strtok for commands.
	while (fgets(input, MAX, ifile) != NULL) // this will handle the push and pop stuff.
	{
		
		pushpop_flag = 1;

		//strtok(string, delimiters), strtok basically helps read in one word at a time. 
		
		token = strtok(input, " \t"); // token represents the command (push, pop, or status).

		// For example, if input = "push %rax", since we have a whitespace and \t as the delimiter,
		// It will read up until the whitespace or delimiter, so it will only read in "push" for the token.


	    // status command
	    if (strcmp(token, "status\n") == 0 || strcmp(token, "status") == 0)
		    stack_status(stackarr, registers, stackptr, hex_bottom, hex_lowadd);

	    // push command
	    else if (strcmp(token, "push") == 0)
	    {
	            if (arrptr == &stackarr[arrsize - 1]) // we are checking if the user is pushing a value but there is no more space in the register.
		    {
			    printf("overflow of stack\n");
			    continue;
		    }

		    token = strtok(NULL, " \t\n");
		    // we can call strtok again to read in the next string. we can actually have it continue by having the first arguement as NULL
		    // so now from the previous example, if we read "push" from "push %rax", it will now read in "%rax" to token.


		    // check if address
		    if (token[0] == '$')
		    {
			    if (is_verbose == 1)
			    {
				    fprintf(stderr, ">> push reg: 0x0%lx %s\n", stackptr, token);
			    }

			    token = strtok(token, "$");

			    //len = strlen(token);
			    
			    /*
			    for (int i = 0; i < len; i++)
			    {
				    if (token[i] > 'f')
					    pushpop_flag = 0;
			    }
			    */

			    if (token[0] > 'f')
				    pushpop_flag = 0;

			    if (pushpop_flag == 0)
			    {
				    printf("cannot parse number\n");
			    }
			    else
			    {
				    stackptr -= 8;
				    arrptr += 1;
				    push_value(token, arrptr);
			    }
		    }

		    // check if reg
		    else if (token[0] == '%')
		    {
			    if (is_verbose == 1)
			    {
				    fprintf(stderr, ">> push reg: 0x0%lx %s\n", stackptr, token);
			    }
			    if (token[2] == 'a' || token[2] == 'b' || token[2] == 'c' || token[2] == 'd')
			    {
				    stackptr -= 8;
				    arrptr += 1;
				    push_reg(token, registers, arrptr);
			    }

			    else
			    {
				    printf("unrecognized register: %s\n", token);
			    }
		    }

		    else
		    {
			    printf("unrecognized operand for push: %s\n", token);
		    }
	    }

	    // pop command
	    else if (strcmp(token, "pop") == 0)
	    {
		    if (arrptr == &stackarr[0])
		    {
			    printf("at bottom of stack\n");
			    continue;
		    }
		    

		   token = strtok(NULL, " \t\n");
		   if (token[0] == '%')
		   {
			   if (is_verbose == 1)
				   fprintf(stderr, ">> pop: 0x0%lx %s\n", stackptr, token);

			   if (token[2] == 'a' || token[2] == 'b' || token[2] == 'c' || token[2] == 'd')
			   {
			           pop(token, registers, arrptr);
				   stackptr += 8;
				   arrptr -= 1;
			   }
			   else
			   {
				   printf("unrecognized register: %s\n", token);
			   }
		   }
		   else
			   printf("unrecognized operand for pop: %s\n", token);
	    }

	    else
	    {
		    if (strcmp(token, "\n") == 0)
			    continue;
		    if (strcmp(token, "pop\n") == 0 && strcmp(token, "push\n") == 0)
			    printf("command not recognized: %s", token);
	    }
	 }
    }

    if (ifile_name != NULL)
	    fclose(ifile);

    if (ofile_name != NULL)
	    fclose(ofile);


    if (stackarr != NULL)
	    free(stackarr);
    
    return EXIT_SUCCESS;
}

