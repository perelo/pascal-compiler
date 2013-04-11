#/*
# * @File Makefile
# *
# * @Date 14-03-2013
# *
# * @Brief makefile rudimentaire pour le projet de compilation
# *
# */
CC = gcc
CFLAGS = -Wall -O3

H_FILES = code3adr.h dico.h lexemes.h lexical.h parcours.h premiers.h registres.h syntabs.h syntaxique.h
C_FILES = code3adr.c compil.c dico.c lexical.c parcours.c registres.c syntabs.c syntaxique.c

OUTPUT_DIR = 

all : $(H_FILES) $(C_FILES)
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_DIR)compil.run

gdb : $(H_FILES) $(C_FILES)
	$(CC) $(CFLAGS) -ggdb $^ -o $(OUTPUT_DIR)compil.run

clean :
	rm -rfv *.run *.o
