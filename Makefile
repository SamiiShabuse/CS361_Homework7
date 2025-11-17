CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
BIN = bin/search
SRC = src/search.cpp src/channel.h

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(BIN) src/search.cpp

clean:
	rm -rf bin