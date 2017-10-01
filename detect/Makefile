
# Makefile
 
CC=gcc
CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -std=c99 -O2 -o
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lm
 
# you should at least create empty file
# order to compile
 
SRC= binarization.c som.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
 
all:
 
-include ${DEP}
 
som: ${OBJ}
 
clean:
	rm -f ${OBJ} ${DEP} listest
 
# END
