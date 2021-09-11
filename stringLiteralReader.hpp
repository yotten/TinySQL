#pragma once

#include "token.hpp"
#include "tokenReader.hpp"

#include <string>
#include <memory>

//! 文字列リテラルトークンを読み込む機能を提供します。
class StringLiteralReader : public TokenReader
{
protected:
	//! トークンを読み込みます。
	//! @param [in] cursol 読み込み開始位置です。
	//! @param [in] end SQL全体の終了位置です。
	//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
    const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const override;
};
