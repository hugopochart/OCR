# Makefile
 
CC=gcc 
CPPFLAGS= -MMD 
CFLAGS= -Wall -Wextra -std=c99 -O2
LDFLAGS= 
LDLIBS=-lm  
 
# order to compile
 
SRC= som.c 
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
 
all:
 
-include ${DEP}
 
som: ${OBJ}
 
clean:
	rm -f ${OBJ} ${DEP} main_tests
 
# END
