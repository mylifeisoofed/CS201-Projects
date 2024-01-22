CC = -gcc
DEBUG = -g
DEFINES = 

CFLAGS = $(DEBUG) $(DEFINES) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition \
	 -Wmissing-prototypes -Wdeclaration-after-statement -Wextra -Werror -Wpedantic

PROG1 = push_pop
PROG2 = stack_status

PROGS = $(PROG1)

all: $(PROGS)

# $@ is the name of the target
# $< ONLY refers to the first prereq.
# $^ referes to ALL the prereq.

$(PROG1): $(PROG1).o $(PROG2).o
	$(CC) $(CFLAGS) -o $@ $^

$(PROG1).o: $(PROG1).c $(PROG1).h
	$(CC) $(CFLAGS) -c $<


$(PROG2).o: $(PROG2).c $(PROG1).h
	$(CC) $(CFLAGS) -c $<

clean cls:
	-rm -f $(PROGS) *.o *~ \#*




