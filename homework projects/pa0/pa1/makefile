include flag.mak

SOURCES=$(wildcard *.cc)
HEADERS=$(wildcard *.h)
OBJECTS=$(SOURCES:%.cc=%.o)
TARGET=main

flags =

ifeq ($(F1),TRUE)
	flags += -D__F1__
endif
ifeq ($(F2),TRUE)
	flags += -D__F2__
endif
ifeq ($(F3),TRUE)
	flags += -D__F3__
endif
ifeq ($(F4),TRUE)
	flags += -D__F4__
endif

all: $(TARGET)
$(TARGET): $(OBJECTS) $(HEADERS) flag.mak
	g++ $(OBJECTS) -o $(TARGET)
%.o: %.cc %.h
	g++ -c $< -o $@ -std=c++17
main.o: main.cc flag.mak
	g++ -c $< -o $@ -std=c++17 $(flags)
clean:
	rm -f $(TARGET) $(OBJECTS)

