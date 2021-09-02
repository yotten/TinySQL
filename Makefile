CFLAGS=-std=c++17 #-Wall
LDFLAGS=-pthread -lgtest_main -lgtest

test: testExecuteSQL.o ExecuteSQL.o data.o operator.o token.o column.o extension_tree_node.o column_index.o sqlQuery.o
	g++ -o testExecuteSQL testExecuteSQL.o ExecuteSQL.o data.o operator.o token.o column.o extension_tree_node.o column_index.o sqlQuery.o $(CFLAGS) $(LDFLAGS)
	./testExecuteSQL

#testExecuteSQL.o: testExecuteSQL.cpp
testExecuteSQL.o: testExecuteSQL.cpp 
	g++ -c $(CFLAGS) testExecuteSQL.cpp

ExecuteSQL.o: ExecuteSQL.cpp data.hpp operator.hpp token.hpp token_kind.hpp column.hpp extension_tree_node.hpp column_index.hpp sqlQuery.hpp inputTable.hpp resultValue.hpp 
	g++ -c $(CFLAGS) ExecuteSQL.cpp

data.o: data.cpp data.hpp
	g++ -c $(CFLAGS) data.cpp

operator.o: operator.cpp operator.hpp
	g++ -c $(CFLAGS) operator.cpp

token.o: token.cpp token.hpp token.hpp
	g++ -c $(CFLAGS) token.cpp

column.o: column.cpp column.hpp 
	g++ -c $(CFLAGS) column.cpp 

extension_tree_node.o: extension_tree_node.cpp extension_tree_node.hpp data.hpp
	g++ -c $(CFLAGS) extension_tree_node.cpp

column_index.o: column_index.cpp column_index.hpp
	g++ -c $(CFLAGS) column_index.cpp

sqlQuery.o: sqlQuery.cpp sqlQuery.hpp sqlQueryInfo.hpp resultValue.hpp
	g++ -c $(CFLAGS) sqlQuery.cpp

clean:
	rm -f *.o

.PHONY: test clean
