#include "keywordReader.hpp"

using namespace std;

//! KeywordReaderクラスの新しいインスタンスを初期化します。
//! @param [in] kind トークンの種類です。
//! @param [in] word キーワードの文字列です。
KeywordReader::KeywordReader(const TokenKind kind, const string word) : keyword(kind, word)
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
        result.second != end && alpahNumUnder.find(*result.second) == string::npos) {
            cursol = result.second;
            return make_shared<Token>(keyword);
    }
    return nullptr;
}