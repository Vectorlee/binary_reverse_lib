# Makefile
#
#


CC  = g++
CPPFLAGS = -g -Wall -I ./xed2-ia32/include -I ./include
LIBS = -L ./xed2-ia32/lib -lxed
VPATH = source include xed2-ia32/include

EXECUTABLE = REVERSE

SRCS = main.cpp       \
       utility.cpp    \
       register.cpp   \
       tracker.cpp    \
       varinfer.cpp 


HDRS = utility.h      \
       register.h     \
       tracker.h      \
       varinfer.h


OBJECTS = utility.o   \
          tracker.o   \
          register.o  \
          varinfer.o  \
          main.o 



all: $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

main.o: main.cpp utility.h varinfer.h
	$(CC) $(CPPFLAGS) -c $<

utility.o: utility.cpp utility.h
	$(CC) $(CPPFLAGS) -c $<

register.o: register.cpp register.h
	$(CC) $(CPPFLAGS) -c $<

tracker.o: tracker.cpp tracker.h
	$(CC) $(CPPFLAGS) -c $<

varinfer.o: varinfer.cpp varinfer.h
	$(CC) $(CPPFLAGS) -c $<



.PHONY: clean
clean: 
	rm -rf $(EXECUTABLE) $(OBJECTS)


