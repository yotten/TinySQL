#include "keywordReader.hpp"

using namespace std;

//! KeywordReaderクラスの新しいインスタンスを初期化します。
//! @param [in] kind トークンの種類です。
//! @param [in] word キーワードの文字列です。
KeywordReader::KeywordReader(const TokenKind kind, const string word) : keyword(Token(kind, word))
{

}

//! 実際にトークンを読み込みます。
//! @param [in] cursol 読み込み開始位置です。
//! @param [in] end SQL全体の終了位置です。
//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
const shared_ptr<const Token> KeywordReader::ReadCore(string::const_iterator &cursol, const string::const_iterator &end) const
{
    auto result = mismatch(keyword.word.begin(), keyword.word.end(), cursol,
    [](const char keywordChar, const char sqlChar){
        return keywordChar == toupper(sqlChar);
    });

    if (result.first == keyword.word.end() &&
        CheckNextChar(result.second, end)) {
            cursol = result.second;
            return make_shared<Token>(keyword);
    }
    return nullptr;
}
//! キーワードの次の文字のチェックを行います。
//! @param [in] next チェック対象となる次の文字のイテレータです。
//! @param [in] next endイテレータです。
const bool KeywordReader::CheckNextChar(const string::const_iterator& next, const string::const_iterator& end) const
{
	//キーワードに識別子が区切りなしに続いていないかを確認します。
    return next != end && alpahNumUnder.find(*next) == string::npos;
}