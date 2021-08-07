//! トークンの種類を表します。
enum class TokenKind
{
	NOT_TOKEN,              //!< トークンを表しません。
	ASC,                    //!< ASCキーワードです。
	AND,                    //!< ANDキーワードです。
	BY,                     //!< BYキーワードです。
	DESC,                   //!< DESCキーワードです。
	FROM,                   //!< FROMキーワードです。
	OR,                     //!< ORキーワードです。
	ORDER,                  //!< ORDERキーワードです。
	SELECT,                 //!< SELECTキーワードです。
	WHERE,                  //!< WHEREキーワードです。
	ASTERISK,               //!< ＊ 記号です。
	COMMA,                  //!< ， 記号です。
	CLOSE_PAREN,            //!< ） 記号です。
	DOT,                    //!< ． 記号です。
	EQUAL,                  //!< ＝ 記号です。
	GREATER_THAN,           //!< ＞ 記号です。
	GREATER_THAN_OR_EQUAL,  //!< ＞＝ 記号です。
	LESS_THAN,              //!< ＜ 記号です。
	LESS_THAN_OR_EQUAL,     //!< ＜＝ 記号です。
	MINUS,                  //!< － 記号です。
	NOT_EQUAL,              //!< ＜＞ 記号です。
	OPEN_PAREN,             //!< （ 記号です。
	PLUS,                   //!< ＋ 記号です。
	SLASH,                  //!< ／ 記号です。
	IDENTIFIER,             //!< 識別子です。
	INT_LITERAL,            //!< 整数リテラルです。
	STRING_LITERAL          //!< 文字列リテラルです。
};

//! WHERE句に指定する演算子の情報を表します。
class Operator {
public:
	TokenKind kind; //!< 演算子の種類を、演算子を記述するトークンの種類で表します。
	int order; //!< 演算子の優先順位です。
};