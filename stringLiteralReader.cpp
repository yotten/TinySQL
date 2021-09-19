#include "stringLiteralReader.hpp"
#include "resultValue.hpp"

#include <algorithm>

using namespace std;

//! トークンを読み込みます。
//! @param [in] cursol 読み込み開始位置です。
//! @param [in] end SQL全体の終了位置です。
//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
const shared_ptr<const Token> StringLiteralReader::ReadCore(string::const_iterator &cursol, const string::const_iterator &end) const
{
    auto start = cursol;

    // 文字列リテラルを開始するシングルクォートを判別し、読み込みます。
    if (*cursol  == "\'"[0]){
        ++cursol;

        // メトリクス測定ツールのccccはシングルクォートの文字リテラル中のエスケープを認識しないため、文字リテラルを使わないことで回避しています。
        cursol = find_if_not(cursol, end, [](char c){return c != "\'"[0];});
        if (cursol == end) {
            throw ResultValue::ERR_TOKEN_CANT_READ;
        }
        ++cursol;

        return make_shared<Token>(TokenKind::STRING_LITERAL, string(start, cursol));
    }

    return nullptr;
}