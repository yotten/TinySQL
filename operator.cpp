#include "operator.hpp"

//! Operatorクラスの新しいインスタンスを初期化します。
Operator::Operator()
{
}

//! Operatorクラスの新しいインスタンスを初期化します。
//! @param [in] kind 演算子の種類を、演算子を記述するトークンの種類で表します。
//! @param [in] order 演算子の優先順位です。
Operator::Operator(const TokenKind kind, const int order) : kind(kind), order(order)
{
}