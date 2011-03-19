CXX=g++
CXXFLAGS=-O3 -Wall -funroll-all-loops -fomit-frame-pointer

performance: main.o
	$(CXX) $(CXXFLAGS) *.o -o performance

clean:
	rm -f *.o performance result
	
