CXX=g++
CFLAGS=-std=c99 -O3 -Wall -funroll-all-loops -fomit-frame-pointer

performance: main.o
	$(CXX) $(CFLAGS) *.o -o performance

clean:
	rm -f *.o performance result
	
