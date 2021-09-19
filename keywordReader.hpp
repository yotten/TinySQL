#pragma once

#include "tokenReader.hpp"

//! キーワードトークンを読み込む機能を提供します。
class KeywordReader : public TokenReader
{
protected:
    Token keyword; //!< 読み込むキーワードトークンと等しいトークンです。

    //! 実際にトークンを読み込みます。
    //! @param [in] cursol 読み込み開始位置です。
    //! @param [in] end SQL全体の終了位置です。
    //! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
    const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const override;
    //! キーワードの次の文字のチェックを行います。
	//! @param [in] next チェック対象となる次の文字のイテレータです。
	//! @param [in] next endイテレータです。
	virtual const bool CheckNextChar(const std::string::const_iterator& next, const std::string::const_iterator& end) const;
public:
	//! KeywordReaderクラスの新しいインスタンスを初期化します。
	//! @param [in] kind トークンの種類です。
	//! @param [in] word キーワードの文字列です。
    KeywordReader(const TokenKind kind, const std::string word);
};