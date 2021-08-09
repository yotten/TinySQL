CFLAGS=-std=c++14 -Wall
LDFLAGS=-pthread -lgtest_main -lgtest

test: testExecuteSQL.o ExecuteSQL.o data.o
	g++ -o testExecuteSQL testExecuteSQL.o ExecuteSQL.o data.o $(CFLAGS) $(LDFLAGS)
	./testExecuteSQL

#testExecuteSQL.o: testExecuteSQL.cpp
testExecuteSQL.o: testExecuteSQL.cpp 
	g++ -c testExecuteSQL.cpp

ExecuteSQL.o: ExecuteSQL.cpp data.hpp operator.hpp token.hpp token_kind.hpp column.hpp extension_tree_node.hpp column_index.hpp
	g++ -c ExecuteSQL.cpp

data.o: data.cpp data.hpp
	g++ -c $(CFLAGS) data.cpp
clean:
	rm *.o

.PHONY: test clean
