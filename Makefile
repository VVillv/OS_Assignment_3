# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Werror -std=c++20

# Build targets
all: firstfit bestfit

firstfit: firstfit.o memory_manager.o
	$(CXX) $(CXXFLAGS) -o firstfit firstfit.o memory_manager.o

bestfit: bestfit.o memory_manager.o
	$(CXX) $(CXXFLAGS) -o bestfit bestfit.o memory_manager.o

firstfit.o: firstfit.cpp memory_manager.h
	$(CXX) $(CXXFLAGS) -c firstfit.cpp

bestfit.o: bestfit.cpp memory_manager.h
	$(CXX) $(CXXFLAGS) -c bestfit.cpp

memory_manager.o: memory_manager.cpp memory_manager.h
	$(CXX) $(CXXFLAGS) -c memory_manager.cpp

clean:
	rm -f *.o firstfit bestfit
