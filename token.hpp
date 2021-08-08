#pragma once

#include "token_kind.hpp"

//! トークンを表します。
class Token {
public:
	TokenKind kind; //!< トークンの種類です。
	char word[MAX_WORD_LENGTH]; //!< 記録されているトークンの文字列です。記録の必要がなければ空白です。
};