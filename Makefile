CXX = g++
CXXFLAGS = -std=c++17 -O2
BIN = bin/search

# Gather all .cpp sources in src/
SRCS := $(wildcard src/*.cpp)

# Place object files in obj/ directory
OBJS := $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJS)

# Put object files into obj/ but compile from src/%.cpp
obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf bin obj