#include "intLiteralReader.hpp"

#include <algorithm>

using namespace std;

//! 数値リテラルトークンを読み込む機能を提供します。
//! @param [in] cursol 読み込み開始位置です。
//! @param [in] end SQL全体の終了位置です。
//! @return 切り出されたトークンです。読み込みが失敗した場合はnullptrを返します。
const shared_ptr<const Token> IntLiteralReader::ReadCore(string::const_iterator &cursol, const string::const_iterator &end) const
{
    auto start = cursol;
    cursol = find_if(cursol, end, [&](char c){return num.find(c) == string::npos; });

    //if (cursol != backPoint && (
    if (start != cursol && (
        alpahUnder.find(*cursol) == string::npos || // 数字の後にすぐに識別子が続くのは紛らわしいので数値リテラルとは扱いません。
        cursol == end)) {
            return make_shared<Token>(TokenKind::INT_LITERAL, string(start, cursol));
    }

    return nullptr;
}