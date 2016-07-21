#
#  Makefile
#      This makefile compiles the code for the data structures 
#      directory for Classic Data Structures in C++, 
#      by Timothy A. Budd.  The user must select the
#      appropriate macros, depending on the compiler used.
#      ILIB is the directory where the include files will be found
#

# Common Macros
ILIB    = .
# added this for Linux
ILIB2   =/usr/include/g++

#g++ on Windows98
OSTYPE = WINDOWS98
# g++ elsewhere
#OSTYPE =
#

# Unix Macros
CC     = g++
CFLAGS = -g -O2 -I$(ILIB) -I$(ILIB2) -D$(OSTYPE)
#CFLAGS = -O2 -I$(ILIB) -I$(ILIB2) -D$(OSTYPE)
EXE    =
RM     = rm -fr

# Borland C++ Macros
#CC     = bcc
#CFLAGS = -I$(ILIB) -P -N -D__GNUG__
#OBJ    = .obj
#EXE    = .exe
#RM     = del



all:	random.o string.o bitvec.o bitvector.o clrandom.o clstring.o gnu-rng.o ranlib.o com.o

# removed split.o from this

bundle: clean
	shar Makefile ReadMe *.h *.C *.c >/tmp/ds

clean:
	$(RM) *.o
	$(RM) ptrepository

clrandom.o: clrandom.C clrandom.h gnu-mlcg.h gnu-rng.h
	$(CC) $(CFLAGS) -c clrandom.C

random.o:	random.C
		$(CC) $(CFLAGS) -c random.C

string.o:	string.C
		$(CC) $(CFLAGS) -c string.C

bitvec.o:	bitvec.C
		$(CC) $(CFLAGS) -c bitvec.C

split.o:	split.C vector.h cstring.h bitvec.h
		$(CC) $(CFLAGS) -c split.C

bitvector.o: bitvector.c bitvector.h
	$(CC) $(CFLAGS) -c bitvector.c

table-test.o: table-test.c table.h clrandom.h
	$(CC) $(CFLAGS) -c table-test.c

table-test: table-test.o clrandom.o 
	$(CC) $(CFLAGS) table-test.o clrandom.o -o table-test

gnu-mlcg.o: gnu-mlcg.h gnu-mlcg.cc
	$(CC) $(CFLAGS) -c gnu-mlcg.cc

gnu-rng.o: gnu-rng.h gnu-rng.cc
	$(CC) $(CFLAGS) -c gnu-rng.cc

table-test3: table-test3.c table.h list.h iterator.h clrandom.h
	$(CC) $(CFLAGS) table-test3.c clrandom.o gnu-rng.o gnu-mlcg.o -o table-test3

ranlib.o: ranlib.c ranlib.h
	gcc $(CFLAGS) -c ranlib.c

com.o: com.c ranlib.h
	gcc $(CFLAGS) -c com.c 

deltaheap-test: deltaheap-test.c deltaheap.h clrandom.o
	$(CC) $(CFLAGS) deltaheap-test.c clrandom.o -o deltaheap-test
