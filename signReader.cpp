#include "signReader.hpp"

using namespace std;

//! KeywordReaderクラスの新しいインスタンスを初期化します。
//! @param [in] kind トークンの種類です。
//! @param [in] word キーワードの文字列です。
SignReader::SignReader(const TokenKind kind, const string word) : KeywordReader(kind, word){}
//! キーワードの次の文字のチェックを行います。
//! @param [in] next チェック対象となる次の文字のイテレータです。
//! @param [in] next endイテレータです。
const bool SignReader::CheckNextChar(const string::const_iterator& next, const string::const_iterator& end) const
{
	// 次の文字はチェックせずに必ずOKとなります。
	return true;
}