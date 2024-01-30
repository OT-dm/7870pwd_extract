TARGET = 7870pwd_extract

CSOURCES=$(wildcard ed25519/src/*.c)
CPPSOURCES=$(wildcard *.cpp)
OBJECTS=$(CPPSOURCES:.cpp=.o) $(CSOURCES:.c=.o)
DEPS=$(OBJECTS:%.o=%.d)

CXX = g++
CC = gcc
CFLAGS = -Wall -g -D_FILE_OFFSET_BITS=64 -MMD #-O2
CXXFLAGS =
LDFLAGS =  -Wl,-lminizip,-lcrypto  -Wl,-Map=output.map    

.PHONY: all

all: $(TARGET)

$(TARGET) : $(OBJECTS)
#	@echo "  LD  $(OBJECTS) $@"
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

# Include all .d files
-include $(DEPS)

%.o: %.cpp
#	@echo "  CXX    $@"
	$(CXX)  $(CFLAGS) $(CXXFLAGS) -c $< -o $@

%.o: %.c
#	@echo "  CC     $@"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(DEPS) $(TARGET) *.map