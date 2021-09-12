#include "tokenReader.hpp"

//! 識別子トークンを読み込む機能を提供します。
class IdentifierReader : public TokenReader
{
protected:
	//! 実際にトークンを読み込みます。
	//! @param [in] cursol 読み込み開始位置です。
	//! @param [in] end SQL全体の終了位置です。
	//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
	const std::shared_ptr<const Token> ReadCore(std::string::const_iterator &cursol, const std::string::const_iterator& end) const override;
};