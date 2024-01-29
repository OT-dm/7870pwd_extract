TARGET = 7870pwd_extract

CSOURCES=$(wildcard ed25519/src/*.c)
CPPSOURCES=$(wildcard *.cpp)
OBJECTS=$(CPPSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

CXX = g++
CC = gcc
CFLAGS = -Wall -O2
CXXFLAGS =
LDFLAGS =  -Wl,-lminizip,-lcrypto  -Wl,-Map=output.map    

.PHONY: all

all: $(TARGET)

$(TARGET) : $(OBJECTS)
#	@echo "  LD  $(OBJECTS) $@"
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
#	@echo "  CXX    $@"
	$(CXX)  $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
#	@echo "  CC     $@"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(TARGET) *.map