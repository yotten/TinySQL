CFLAGS=-std=c++14
LDFLAGS=-pthread -lgtest_main -lgtest

test: testExecuteSQL.o ExecuteSQL.o
	g++ -o testExecuteSQL testExecuteSQL.o ExecuteSQL.o $(CFLAGS) $(LDFLAGS)
	./testExecuteSQL

#testExecuteSQL.o: testExecuteSQL.cpp
testExecuteSQL.o: testExecuteSQL.cpp 
	g++ -c testExecuteSQL.cpp

ExecuteSQL.o: ExecuteSQL.c
	gcc -c ExecuteSQL.c

clean:
	rm *.o

.PHONY: test clean
