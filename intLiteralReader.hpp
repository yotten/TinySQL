#pragma once

#include "token.hpp"
#include "tokenReader.hpp"

#include <string>
#include <memory>

//! 数値リテラルトークンを読み込む機能を提供します。
class IntLiteralReader : public TokenReader
{
protected:
    //! トークンを読み込みます。
    //! @param [in] cursol 読み込み開始位置です。
    const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const override;
};