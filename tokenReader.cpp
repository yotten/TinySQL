#include "tokenReader.hpp"

using namespace std;

const shared_ptr<const Token> TokenReader::Read(string::const_iterator &cursol, const string::const_iterator &end) const
{
	auto backPoint = cursol;
	auto token = ReadCore(cursol, end);

	if (!token) {
		cursol = backPoint;
	}

	return token;
}