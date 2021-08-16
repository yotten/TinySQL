CFLAGS=-std=c++14 -Wall
LDFLAGS=-pthread -lgtest_main -lgtest

test: testExecuteSQL.o ExecuteSQL.o data.o operator.o token.o column.o extension_tree_node.o column_index.o
	g++ -o testExecuteSQL testExecuteSQL.o ExecuteSQL.o data.o operator.o token.o column.o extension_tree_node.o column_index.o $(CFLAGS) $(LDFLAGS)
	./testExecuteSQL

#testExecuteSQL.o: testExecuteSQL.cpp
testExecuteSQL.o: testExecuteSQL.cpp 
	g++ -c testExecuteSQL.cpp

ExecuteSQL.o: ExecuteSQL.cpp data.hpp operator.hpp token.hpp token_kind.hpp column.hpp extension_tree_node.hpp column_index.hpp
	g++ -c ExecuteSQL.cpp

data.o: data.cpp data.hpp
	g++ -c $(CFLAGS) data.cpp

operator.o: operator.cpp operator.hpp
	g++ -c operator.cpp

token.o: token.cpp token.hpp token.hpp
	g++ -c token.cpp

column.o: column.cpp column.hpp 
	g++ -c $(CFLAGS) column.cpp 

extension_tree_node.o: extension_tree_node.cpp extension_tree_node.hpp
	g++ -c extension_tree_node.cpp

column_index.o: column_index.cpp column_index.hpp
	g++ -c column_index.cpp
clean:
	rm -f *.o

.PHONY: test clean
