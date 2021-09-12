#include "signReader.hpp"

using namespace std;

//! 実際にトークンを読み込みます。
//! @param [in] cursol 読み込み開始位置です。
//! @param [in] end SQL全体の終了位置です。
//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
const shared_ptr<const Token> SignReader::ReadCore(string::const_iterator &cursol, const string::const_iterator &end) const
{
	auto result =
		mismatch(sign.word.begin(), sign.word.end(), cursol,
		[](const char keywordChar, const char sqlChar){return keywordChar == toupper(sqlChar); });

	if (result.first == sign.word.end()){
		cursol = result.second;
		return make_shared<Token>(sign);
	}
	else{
		return nullptr;
	}
}

//! KeywordReaderクラスの新しいインスタンスを初期化します。
//! @param [in] kind トークンの種類です。
//! @param [in] word キーワードの文字列です。
SignReader::SignReader(const TokenKind kind, const string word) : sign(Token(kind, word)){}