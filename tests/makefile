CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -I../include -I../yymp/include

OBJ=fun.o
BIN=fun.exe

all: $(OBJ) $(BIN)

%.exe: %.o
	$(CXX) $(CXXFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: wclean
wclean:
	del /F /Q *.o
	del /F /Q *.exe

.PHONY: uclean
uclean:
	rm *.o
	rm *.exe
