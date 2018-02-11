CC      = g++
CFLAGS  = -c -Wall -fsanitize=address --std=c++14
LDFLAGS = -fsanitize=address

SOURCES    = test/main.cpp src/lisp.cpp
LIBS       = src/lisp.h src/any.h
OBJECTS    = $(SOURCES:.cpp=.o)
EXECUTABLE = lisp

all: $(SOURCES) $(EXECUTABLE) clean

$(EXECUTABLE): $(OBJECTS) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o
