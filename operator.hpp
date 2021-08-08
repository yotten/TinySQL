#pragma once

#include "token_kind.hpp"

//! WHERE句に指定する演算子の情報を表します。
class Operator {
public:
	TokenKind kind; //!< 演算子の種類を、演算子を記述するトークンの種類で表します。
	int order; //!< 演算子の優先順位です。
};