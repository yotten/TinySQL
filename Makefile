CFLAGS=-std=c++14
LDFLAGS=-pthread -lgtest_main -lgtest

test: testExecuteSQL.o ExecuteSQL.o
	g++ -o testExecuteSQL testExecuteSQL.o ExecuteSQL.o $(CFLAGS) $(LDFLAGS)
	./testExecuteSQL

#testExecuteSQL.o: testExecuteSQL.cpp
testExecuteSQL.o: testExecuteSQL.cpp data.hpp operator.hpp token.hpp token_kind.hpp
	g++ -c testExecuteSQL.cpp

ExecuteSQL.o: ExecuteSQL.cpp data.hpp operator.hpp token.hpp token_kind.hpp
	g++ -c ExecuteSQL.cpp

clean:
	rm *.o

.PHONY: test clean
