# Simple makefile to compile avr test runners to the host machine

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# The name of the file to build
FILE = 

# Name of the output file
OUTFILE = 

all:
	$(CXX) $(CXXFLAGS) $(FILE) -o $(OUTFILE) -I../submodules/simavr/simavr/

clean:
	rm -f $(OUTFILE)