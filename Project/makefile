

CC  = g++
CFLAGS = -g -Wall -I ./xed2-ia32/include
SRCS = src/test.cpp
LIBS = -L ./xed2-ia32/lib -lxed
EXECUTABLE = test

all: $(SRCS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SRCS) $(LIBS)


