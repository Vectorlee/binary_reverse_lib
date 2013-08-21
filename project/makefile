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
       trailer.cpp    \
       varinfer.cpp   \
       typeinfer.cpp  \
       constraint.cpp \
       typeslvr.cpp   \
       extract.cpp
	


HDRS = utility.h      \
       register.h     \
       tracker.h      \
       trailer.h      \
       varinfer.h     \
       typeinfer.h    \
       constraint.h   \
       typeslvr.h     \
       extract.h



OBJECTS = utility.o    \
          tracker.o    \
          trailer.o    \
          register.o   \
          varinfer.o   \
          typeinfer.o  \
          constraint.o \
          extract.o    \
          typeslvr.o   \
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

trailer.o: trailer.cpp trailer.h
	$(CC) $(CPPFLAGS) -c $<

varinfer.o: varinfer.cpp varinfer.h
	$(CC) $(CPPFLAGS) -c $<

typeinfer.o: typeinfer.cpp typeinfer.h
	$(CC) $(CPPFLAGS) -c $<

constraint.o: constraint.cpp constraint.h
	$(CC) $(CPPFLAGS) -c $<

extract.o: extract.cpp extract.h
	$(CC) $(CPPFLAGS) -c $<

typeslvr.o: typeslvr.cpp typeslvr.h
	$(CC) $(CPPFLAGS) -c $<


.PHONY: clean
clean: 
	rm -rf $(EXECUTABLE) $(OBJECTS)


