all:
	make -C c
	make -C c++
	make -C java
	make -C js

clean:
	make -C c clean
	make -C c++ clean
	make -C java clean
	make -C js clean

test: all
	./test

report: test
	node gen_report.js

