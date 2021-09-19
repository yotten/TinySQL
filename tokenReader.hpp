#pragma once

#include "token.hpp"

#include <string>
#include <memory>

class TokenReader {
protected:
    const std::string alpahUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ"; //!< 全てのアルファベットの大文字小文字とアンダーバーです。
	const std::string alpahNumUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; //!< 全ての数字とアルファベットの大文字小文字とアンダーバーです。
	const std::string num = "0123456789"; //!< 全ての数字です。

	//! 実際にトークンを読み込ます。
	//! @param [in] cursol 読み込み開始位置です。
	//! @param [in] end SQL全体の終了位置です。
	//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
    virtual const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const = 0;

public:
	//! トークンを読み込みます。
	//! @param [in] cursol 読み込み開始位置です。
	//! @param [in] end SQL全体の終了位置です。
	//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
	const std::shared_ptr<const Token> Read(std::string::const_iterator &cursol, const std::string::const_iterator& end) const;
};