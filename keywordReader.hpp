#include "tokenReader.hpp"

//! キーワードトークンを読み込む機能を提供します。
class KeywordReader : public TokenReader
{
    Token keyword; //!< 読み込むキーワードトークンと等しいトークンです。

protected:
    //! 実際にトークンを読み込みます。
    //! @param [in] cursol 読み込み開始位置です。
    //! @param [in] end SQL全体の終了位置です。
    //! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
    const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const override;
public:
	//! KeywordReaderクラスの新しいインスタンスを初期化します。
	//! @param [in] kind トークンの種類です。
	//! @param [in] word キーワードの文字列です。
    KeywordReader(const TokenKind kind, const std::string word);
};