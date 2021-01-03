CC = g++
CFLAGS = --std=c++17




Instruction.o: \
	Instruction.cpp
	$(CC) $(CFLAGS) -c Instruction.cpp -o $@


Parser.o:
	$(CC) $(CFLAGS) -o $@ -c Parser.cpp

Code.o: \
	Parser.o
	$(CC) $(CFLAGS) -o $@ -c Code.cpp

State.o: \
	Instruction.o \
	GC.o
	$(CC) $(CFLAGS) -o $@ -c State.cpp

GC.o:
	$(CC) $(CFLAGS) -o $@ -c GC.cpp

VM.o: \
	State.o \
	Instruction.o \
	GC.o
	$(CC) $(CFLAGS) -o $@ -c VM.cpp

Main.o: Main.cpp \
	Instruction.o \
	Parser.o \
	Code.o \
	State.o \
	GC.o \
	VM.o
	$(CC) $(CFLAGS) -o a.out $^


all: Main.o

run: Main.o
	./a.out

re: clean all
	./a.out

clean:
	rm -f *.o
	rm -f *.out
