CC = gcc 
CFLAGS = -g -ggdb -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
PROG = bigFiles
PROG2 = BennySh

DEFINES = DTEST_MODE

all: $(PROG) $(PROG2)


bigFiles: bigFiles.o
	$(CC) $(CFLAGS) -o bigFiles bigFiles.o 


bigFiles.o: bigFiles.c 
	$(CC) $(CFLAGS) -c  bigFiles.c

BennySh: BennySh.o
	$(CC) $(CFLAGS) -o BennySh BennySh.o 


BennySh.o: BennySh.c 
	$(CC) $(CFLAGS) -c  BennySh.c cmd_parse.h


# csv2bin: csv2bin.
# 	$(CC) $(CFLAGS) -o csv2bin csv2bin.o 

# csv2bin.o: csv2bin.c 
# 	$(CC) $(CFLAGS) -c  csv2bin.c

# bin2csv: bin2csv.o
# 	$(CC) $(CFLAGS) -o bin2csv bin2csv.o 


# bin2csv.o: bin2csv.c 
# 	$(CC) $(CFLAGS) -c  bin2csv.c



clean:
	rm -f $(PROG) $(PROG2) *.o *~ \#*