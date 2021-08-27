//! @file
#include "data.hpp"
#include "operator.hpp"
#include "token.hpp"
#include "extension_tree_node.hpp"
#include "column_index.hpp"

#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <memory>
#include "ExecuteSQL.hpp"

//#pragma warning(disable:4996)

#define MAX_COLUMN_COUNT 16                //!< 入出力されるデータに含まれる列の最大数です。
#define MAX_TABLE_COUNT 8                  //!< CSVとして入力されるテーブルの最大数です。

using namespace std;

//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
//! @param [in] sql 実行するSQLです。
//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
//! @return 実行した結果の状態です。
int ExecuteSQL(const string, const string);

//! ExecuteSQLの戻り値の種類を表します。
enum class ResultValue : int {
	OK = 0,                     //!< 問題なく終了しました。
	ERR_FILE_OPEN = 1,          //!< ファイルを開くことに失敗しました。
	ERR_FILE_WRITE = 2,         //!< ファイルに書き込みを行うことに失敗しました。
	ERR_FILE_CLOSE = 3,         //!< ファイルを閉じることに失敗しました。
	ERR_TOKEN_CANT_READ = 4,    //!< トークン解析に失敗しました。
	ERR_SQL_SYNTAX = 5,         //!< SQLの構文解析が失敗しました。
	ERR_BAD_COLUMN_NAME = 6,    //!< テーブル指定を含む列名が適切ではありません。
	ERR_WHERE_OPERAND_TYPE = 7, //!< 演算の左右の型が適切ではありません。
	ERR_CSV_SYNTAX = 8,         //!< CSVの構文解析が失敗しました。
	ERR_MEMORY_ALLOCATE = 9,    //!< メモリの取得に失敗しました。
	ERR_MEMORY_OVER = 10        //!< 用意したメモリ領域の上限を超えました。
};


//! 二つの文字列を、大文字小文字を区別せずに比較し、等しいかどうかです。
//! @param [in] str1 比較される一つ目の文字列です。
//! @param [in] str2 比較される二つ目の文字列です。
//! @return 比較した結果、等しいかどうかです。
static bool Equali(const string str1, const string str2)
{
	bool ret;

	ret = str1.size() == str2.size() &&
		  equal(str1.begin(), str1.end(), str2.begin(),
		  [](const char &c1, const char &c2) {
			  return toupper(c1) == toupper(c2);
		  });

	return ret;
}

//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
//! @param [in] sql 実行するSQLです。
//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
//! @return 実行した結果の状態です。
//! @retval OK=0                      問題なく終了しました。
//! @retval ERR_FILE_OPEN=1           ファイルを開くことに失敗しました。
//! @retval ERR_FILE_WRITE=2          ファイルに書き込みを行うことに失敗しました。
//! @retval ERR_FILE_CLOSE=3          ファイルを閉じることに失敗しました。
//! @retval ERR_TOKEN_CANT_READ=4     トークン解析に失敗しました。
//! @retval ERR_SQL_SYNTAX=5          SQLの構文解析が失敗しました。
//! @retval ERR_BAD_COLUMN_NAME=6     テーブル指定を含む列名が適切ではありません。
//! @retval ERR_WHERE_OPERAND_TYPE=7  演算の左右の型が適切ではありません。
//! @retval ERR_CSV_SYNTAX=8          CSVの構文解析が失敗しました。
//! @retval ERR_MEMORY_ALLOCATE=9     メモリの取得に失敗しました。
//! @retval ERR_MEMORY_OVER=10        用意したメモリ領域の上限を超えました。
//! @details 
//! 参照するテーブルは、テーブル名.csvの形で作成します。                                                     @n
//! 一行目はヘッダ行で、その行に列名を書きます。                                                             @n
//! 前後のスペース読み飛ばしやダブルクォーテーションでくくるなどの機能はありません。                         @n
//! 列の型の定義はできないので、列のすべてのデータの値が数値として解釈できる列のデータを整数として扱います。 @n
//! 実行するSQLで使える機能を以下に例としてあげます。                                                        @n
//! 例1:                                                                                                     @n
//! SELECT *                                                                                                 @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例2: 大文字小文字は区別しません。                                                                        @n
//! select *                                                                                                 @n
//! from users                                                                                               @n
//!                                                                                                          @n
//! 例3: 列の指定ができます。                                                                                @n
//! SELECT Id, Name                                                                                          @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例4: テーブル名を指定して列の指定ができます。                                                            @n
//! SELECT USERS.Id                                                                                          @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例5: ORDER句が使えます。                                                                                 @n
//! SELECT *                                                                                                 @n
//! ORDER BY NAME                                                                                            @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例6: ORDER句に複数列や昇順、降順の指定ができます。                                                       @n
//! SELECT *                                                                                                 @n
//! ORDER BY AGE DESC, Name ASC                                                                              @n
//!                                                                                                          @n
//! 例7: WHERE句が使えます。                                                                                 @n
//! SELECT *                                                                                                 @n
//! WHERE AGE >= 20                                                                                          @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例8: WHERE句では文字列の比較も使えます。                                                                 @n
//! SELECT *                                                                                                 @n
//! WHERE NAME >= 'N'                                                                                        @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例9: WHERE句には四則演算、カッコ、AND、ORなどを含む複雑な式が利用できます。                              @n
//! SELECT *                                                                                                 @n
//! WHERE AGE >= 20 AND (AGE <= 40 || WEIGHT < 100)                                                          @n
//! FROM USERS                                                                                               @n
//!                                                                                                          @n
//! 例10: FROM句に複数のテーブルが指定できます。その場合はクロスで結合します。                               @n
//! SELECT *                                                                                                 @n
//! FROM USERS, CHILDREN                                                                                     @n
//!                                                                                                          @n
//! 例11: WHEREで条件をつけることにより、テーブルの結合ができます。                                          @n
//! SELECT USERS.NAME, CHILDREN.NAME                                                                         @n
//! WHERE USERS.ID = CHILDREN.PARENTID                                                                       @n
//! FROM USERS, CHILDREN                                                                                     @n
int ExecuteSQL(const string sql, const string outputFileName)
{
	vector<ifstream> inputTableFiles;							// 読み込むファイルの全ての入力ストリーム
	ofstream outputFile;										// 書き込むファイルのファイルポインタです。
	bool found = false;                                     // 検索時に見つかったかどうかの結果を一時的に保存します。
	const char *search = nullptr;                              // 文字列検索に利用するポインタです。
	vector<vector<vector<Data>>> inputData;						// 入力データです。
	vector<vector<Data>> outputData;							// 出力データです。
	vector<vector<Data>> allColumnOutputData;					// 出力するデータに対応するインデックスを持ち、すべての入力データを保管します。
	const string alpahUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 全てのアルファベットの大文字小文字とアンダーバーです。
	const string alpahNumUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // 全ての数字とアルファベットの大文字小文字とアンダーバーです。
	const string signNum = "+-0123456789"; // 全ての符号と数字です。
	const string num = "0123456789"; // 全ての数字です。
	const string space = " \t\r\n"; // 全ての空白文字です。

	// SQLからトークンを読み込みます。

	// keywordConditionsとsignConditionsは先頭から順に検索されるので、前方一致となる二つの項目は順番に気をつけて登録しなくてはいけません。

	// キーワードをトークンとして認識するためのキーワード一覧情報です。
	const vector<Token> keywordConditions =
	{
		{ TokenKind::AND, "AND" },
		{ TokenKind::ASC, "ASC" },
		{ TokenKind::BY, "BY" },
		{ TokenKind::DESC, "DESC" },
		{ TokenKind::FROM, "FROM" },
		{ TokenKind::ORDER, "ORDER" },
		{ TokenKind::OR, "OR" },
		{ TokenKind::SELECT, "SELECT" },
		{ TokenKind::WHERE, "WHERE" },
	};

	// 記号をトークンとして認識するための記号一覧情報です。
	//const Token signConditions[] =
	const vector<Token> signConditions =
	{
		{ TokenKind::GREATER_THAN_OR_EQUAL, ">=" },
		{ TokenKind::LESS_THAN_OR_EQUAL, "<=" },
		{ TokenKind::NOT_EQUAL, "<>" },
		{ TokenKind::ASTERISK, "*" },
		{ TokenKind::COMMA, "," },
		{ TokenKind::CLOSE_PAREN, ")" },
		{ TokenKind::DOT, "." },
		{ TokenKind::EQUAL, "=" },
		{ TokenKind::GREATER_THAN, ">" },
		{ TokenKind::LESS_THAN, "<" },
		{ TokenKind::MINUS, "-" },
		{ TokenKind::OPEN_PAREN, "(" },
		{ TokenKind::PLUS, "+" },
		{ TokenKind::SLASH, "/" },
	};

	vector<Token> tokens; // SQLを分割したトークンです。

	// 演算子の情報です。
	const vector<Operator> operators = {
		{ TokenKind::ASTERISK, 1 },
		{ TokenKind::SLASH, 1 },
		{ TokenKind::PLUS, 2 },
		{ TokenKind::MINUS, 2 },
		{ TokenKind::EQUAL, 3 },
		{ TokenKind::GREATER_THAN, 3 },
		{ TokenKind::GREATER_THAN_OR_EQUAL, 3 },
		{ TokenKind::LESS_THAN, 3 },
		{ TokenKind::LESS_THAN_OR_EQUAL, 3 },
		{ TokenKind::NOT_EQUAL, 3 },
		{ TokenKind::AND, 4 },
		{ TokenKind::OR, 5 },
	};

	vector<string> tableNames;

	vector<TokenKind> orders;
	vector<vector<vector<Data>>::iterator> currentRows; // 入力された各テーブルの、現在出力している行を指すカーソルです。
	shared_ptr<ExtensionTreeNode> whereTopNode; // 式木の根となるノードです。
	bool first = true; // FROM句の最初のテーブル名を読み込み中かどうかです。
	//Token *tokenCursol; 	// 現在見ているトークンを指します。
	bool readWhere = false; // すでにWHERE句が読み込み済みかどうかです。
	bool readOrder = false; // すでにORDER句が読み込み済みかどうかです。

	try {
		auto sqlBackPoint = sql.begin(); // SQLをトークンに分割して読み込む時に戻るポイントを記録しておきます。
		auto sqlCursol = sql.begin(); // SQLをトークンに分割して読み込む時に現在読んでいる文字の場所を表します。
		auto sqlEnd = sql.end(); // sqlのendを指します。

		// SQLをトークンに分割て読み込みます。
		while (sqlCursol != sqlEnd){

			// 空白を読み飛ばします。
			sqlCursol = find_if(sqlCursol, sqlEnd, [&](char c){return space.find(c) == string::npos;});
			if (sqlCursol == sqlEnd) {
				break;
			}

			// 数値リテラルを読み込みます。

			// 先頭文字が数字であるかどうかを確認します。
			sqlBackPoint = sqlCursol;
			sqlCursol = find_if(sqlCursol, sqlEnd, [&](char c){return num.find(c) == string::npos;});
			if (sqlCursol != sqlBackPoint && (
				alpahUnder.find(*sqlCursol) == string::npos || // 数字の後にすぐに識別子が続くのは紛らわしいので数値リテラルとは扱いません。
				sqlCursol == sqlEnd)) {
					tokens.push_back(Token(TokenKind::INT_LITERAL, string(sqlBackPoint, sqlCursol)));
					continue;
			}
			else {
				sqlCursol = sqlBackPoint;
			}

			// 文字列リテラルを読み込みます。
			sqlBackPoint = sqlCursol;

			// 文字列リテラルを開始するシングルクォートを判別し、読み込みます。
			// メトリクス測定ツールのccccはシングルクォートの文字リテラル中のエスケープを認識しないため、文字リテラルを使わないことで回避しています。
			if (*sqlCursol  == "\'"[0]){
				++sqlCursol;

				// メトリクス測定ツールのccccはシングルクォートの文字リテラル中のエスケープを認識しないため、文字リテラルを使わないことで回避しています。
				sqlCursol = find_if_not(sqlCursol, sqlEnd, [](char c){return c != "\'"[0];});
				if (sqlCursol == sqlEnd) {
					throw ResultValue::ERR_TOKEN_CANT_READ;
				}
				++sqlCursol;

				tokens.push_back(Token(TokenKind::STRING_LITERAL, string(sqlBackPoint, sqlCursol)));
				continue;
			}

			// キーワードを読み込みます。
			found = false;

			auto keyword = find_if(keywordConditions.begin(), keywordConditions.end(),
				[&](Token keyword) {
					auto result = mismatch(keyword.word.begin(), keyword.word.end(), sqlCursol,
						[](const char keywordChar, const char sqlChar){return keywordChar == toupper(sqlChar);});
					
					if (result.first == keyword.word.end() &&
						result.second != sqlEnd && alpahNumUnder.find(*result.second) == string::npos) {
							sqlCursol = result.second;
					}
					else {
						return false;
					}
			});
			if (keyword != keywordConditions.end()){
				tokens.push_back(Token(keyword->kind));
				continue;
			}

			// 記号を読み込みます。
			auto sign = find_if(signConditions.begin(), signConditions.end(),
				[&](Token keyword) {
					auto result = mismatch(keyword.word.begin(), keyword.word.end(), sqlCursol,
						[](const char keywordChar, const char sqlChar){return keywordChar == toupper(sqlChar);});

					if (result.first == keyword.word.end()) {
						sqlCursol = result.second;
						return true;
					}
					else {
						return false;
					}
				}
			);
			
			if (sign != signConditions.end()) {
				tokens.push_back(Token(sign->kind));
				continue;
			}

			// 識別子を読み込みます。
			sqlBackPoint = sqlCursol;
			if (alpahUnder.find(*sqlCursol++) != string::npos) {
				sqlCursol = find_if(sqlCursol, sqlEnd, [&](const char c){return alpahNumUnder.find(c) == string::npos;});
				tokens.push_back(Token(TokenKind::IDENTIFIER, string(sqlBackPoint, sqlCursol)));
				continue;
			}

			throw ResultValue::ERR_TOKEN_CANT_READ;
		}

		// トークン列を解析し、構文を読み取ります。
		auto tokenCursol = tokens.begin();
		vector<Column> selectColumns; // SELECT句に指定された列名です。
		vector<Column> orderByColumns; // ORDER句に指定された列名です。
		vector<shared_ptr<ExtensionTreeNode>> whereExtensionNodes;

		// SQLの構文を解析し、必要な情報を取得します。

		// SELECT句を読み込みます。
		if (tokenCursol->kind == TokenKind::SELECT){
			++tokenCursol;
		}
		else{
			throw ResultValue::ERR_SQL_SYNTAX;
		}

		if (tokenCursol->kind == TokenKind::ASTERISK){
			++tokenCursol;
		}
		else
		{
			bool first = true; // SELECT句に最初に指定された列名の読み込みかどうかです。
			while (tokenCursol->kind == TokenKind::COMMA || first){
				if (tokenCursol->kind == TokenKind::COMMA){
					++tokenCursol;
				}
				if (tokenCursol->kind == TokenKind::IDENTIFIER){
					// テーブル名が指定されていない場合と仮定して読み込みます。
					selectColumns.push_back(Column(tokenCursol->word.c_str()));
					++tokenCursol;
					if (tokenCursol->kind == TokenKind::DOT){
						++tokenCursol;
						if (tokenCursol->kind == TokenKind::IDENTIFIER){
							// テーブル名が指定されていることがわかったので読み替えます。
							selectColumns.back() = Column(selectColumns.back().columnName, tokenCursol->word);
							++tokenCursol;
						}
						else{
							throw ResultValue::ERR_SQL_SYNTAX;
						}
					}
				}
				else{
					throw ResultValue::ERR_SQL_SYNTAX;
				}
				first = false;
			}
		}

		// ORDER句とWHERE句を読み込みます。最大各一回ずつ書くことができます。
		readOrder = false; // すでにORDER句が読み込み済みかどうかです。
		readWhere = false; // すでにWHERE句が読み込み済みかどうかです。
		while (tokenCursol->kind == TokenKind::ORDER || tokenCursol->kind == TokenKind::WHERE){

			// 二度目のORDER句はエラーです。
			if (readOrder && tokenCursol->kind == TokenKind::ORDER){
				throw ResultValue::ERR_SQL_SYNTAX;
			}

			// 二度目のWHERE句はエラーです。
			if (readWhere && tokenCursol->kind == TokenKind::WHERE){
				throw ResultValue::ERR_SQL_SYNTAX;
			}
			// ORDER句を読み込みます。
			if (tokenCursol->kind == TokenKind::ORDER){
				readOrder = true;
				++tokenCursol;
				if (tokenCursol->kind == TokenKind::BY){
					++tokenCursol;
					bool first = true; // ORDER句の最初の列名の読み込みかどうかです。
					while (tokenCursol->kind == TokenKind::COMMA || first){
						if (tokenCursol->kind == TokenKind::COMMA){
							++tokenCursol;
						}
						if (tokenCursol->kind == TokenKind::IDENTIFIER){
							// テーブル名が指定されていない場合と仮定して読み込みます。
							orderByColumns.push_back(Column(tokenCursol->word));
							++tokenCursol;
							if (tokenCursol->kind == TokenKind::DOT){
								++tokenCursol;
								if (tokenCursol->kind == TokenKind::IDENTIFIER) {
									// テーブル名が指定されていることがわかったので読み替えます。
									orderByColumns.back() = Column(orderByColumns.back().columnName, tokenCursol->word);
									++tokenCursol;
								}
								else{
									throw ResultValue::ERR_SQL_SYNTAX;
								}
							}

							// 並び替えの昇順、降順を指定します。
							if (tokenCursol->kind == TokenKind::ASC) {
								orders.push_back(TokenKind::ASC);
								++tokenCursol;
							}
							else if (tokenCursol->kind == TokenKind::DESC) {
								orders.push_back(TokenKind::DESC);
								++tokenCursol;
							}
							else {
								// 指定がない場合は昇順となります。
								orders.push_back(TokenKind::ASC);
							}
						}
						else{
							throw ResultValue::ERR_SQL_SYNTAX;
						}
						first = false;
					}
				}
				else{
					throw ResultValue::ERR_SQL_SYNTAX;
				}
			}

			// WHERE句を読み込みます。
			if (tokenCursol->kind == TokenKind::WHERE){
				readWhere = true;
				++tokenCursol;
				shared_ptr<ExtensionTreeNode> currentNode; // 現在読み込んでいるノードです。
				while (true){
					// オペランドを読み込みます。

					// オペランドのノードを新しく生成します。
					whereExtensionNodes.push_back(make_shared<ExtensionTreeNode>());
					if (currentNode){
						// 現在のノードを右の子にずらし、元の位置に新しいノードを挿入します。
						currentNode->right = whereExtensionNodes.back();
						currentNode->right->parent = currentNode;
						currentNode = currentNode->right;
					}
					else{
						// 最初はカレントノードに新しいノードを入れます。
						currentNode = whereExtensionNodes.back();
					}

					// カッコ開くを読み込みます。
					while (tokenCursol->kind == TokenKind::OPEN_PAREN){
						++currentNode->parenOpenBeforeClose;
						++tokenCursol;
					}

					// オペランドに前置される+か-を読み込みます。
					if (tokenCursol->kind == TokenKind::PLUS || tokenCursol->kind == TokenKind::MINUS){

						// +-を前置するのは列名と数値リテラルのみです。
						if (tokenCursol[1].kind != TokenKind::IDENTIFIER && tokenCursol[1].kind != TokenKind::INT_LITERAL){
							throw ResultValue::ERR_WHERE_OPERAND_TYPE;
						}
						if (tokenCursol->kind == TokenKind::MINUS){
							currentNode->signCoefficient = -1;
						}
						++tokenCursol;
					}

					// 列名、整数リテラル、文字列リテラルのいずれかをオペランドとして読み込みます。
					if (tokenCursol->kind == TokenKind::IDENTIFIER){

						// テーブル名が指定されていない場合と仮定して読み込みます。
						currentNode->column = Column(tokenCursol->word);
						++tokenCursol;
						if (tokenCursol->kind == TokenKind::DOT){
							++tokenCursol;
							if (tokenCursol->kind == TokenKind::IDENTIFIER){

								// テーブル名が指定されていることがわかったので読み替えます。
								currentNode->column = Column(currentNode->column.columnName, tokenCursol->word);
								++tokenCursol;
							}
							else{
								throw ResultValue::ERR_SQL_SYNTAX;
							}
						}
					}
					else if (tokenCursol->kind == TokenKind::INT_LITERAL){
						currentNode->value = Data(stoi(tokenCursol->word));
						++tokenCursol;
					}
					else if (tokenCursol->kind == TokenKind::STRING_LITERAL){
						// 前後のシングルクォートを取り去った文字列をデータとして読み込みます。
						currentNode->value = Data(string(tokenCursol->word).substr(1, string(tokenCursol->word).size() - 2));

						++tokenCursol;
					}
					else{
						throw ResultValue::ERR_SQL_SYNTAX;
					}

					// オペランドの右のカッコ閉じるを読み込みます。
					while (tokenCursol->kind == TokenKind::CLOSE_PAREN){
						shared_ptr<ExtensionTreeNode> searchedAncestor = currentNode->parent; // カッコ閉じると対応するカッコ開くを両方含む祖先ノードを探すためのカーソルです。
						while (searchedAncestor){

							// searchedAncestorの左の子に対応するカッコ開くがないかを検索します。
							shared_ptr<ExtensionTreeNode> searched = searchedAncestor; // searchedAncestorの内部からカッコ開くを検索するためのカーソルです。
							while (searched && !searched->parenOpenBeforeClose){
								searched = searched->left;
							}
							if (searched){
								// 対応付けられていないカッコ開くを一つ削除し、ノードがカッコに囲まれていることを記録します。
								--searched->parenOpenBeforeClose;
								searchedAncestor->inParen = true;
								break;
							}
							else{
								searchedAncestor = searchedAncestor->parent;
							}
						}
						++tokenCursol;
					}

					// 演算子(オペレーターを読み込みます。
					Operator middleOperator;
					// 現在見ている演算子の情報を探します。
					found = false;
					for (auto &anOperator : operators) {
						if (anOperator.kind == tokenCursol->kind) {
							middleOperator = anOperator;
							found = true;
							break;
						}
					}
					if (found)
					{
						// 見つかった演算子の情報をもとにノードを入れ替えます。
						shared_ptr<ExtensionTreeNode> tmp = currentNode; //ノードを入れ替えるために使う変数です。
						shared_ptr<ExtensionTreeNode> searched = tmp; // 入れ替えるノードを探すためのカーソルです。

						//カッコにくくられていなかった場合に、演算子の優先順位を参考に結合するノードを探します。
						bool first = true; // 演算子の優先順位を検索する最初のループです。
						do{
							if (!first){
								tmp = tmp->parent;
								searched = tmp;
							}
							// 現在の読み込み場所をくくるカッコが開く場所を探します。
							while (searched && !searched->parenOpenBeforeClose){
								searched = searched->left;
							}
							first = false;
						} while (!searched && tmp->parent && (tmp->parent->middleOperator.order <= middleOperator.order || tmp->parent->inParen));

						// 演算子のノードを新しく生成します。
						whereExtensionNodes.push_back(make_shared<ExtensionTreeNode>());
						currentNode = whereExtensionNodes.back();
						currentNode->middleOperator = middleOperator;

						// 見つかった場所に新しいノードを配置します。これまでその位置にあったノードは左の子となるよう、親ノードと子ノードのポインタをつけかえます。
						currentNode->parent = tmp->parent;
						if (currentNode->parent){
							currentNode->parent->right = currentNode;
						}
						currentNode->left = tmp;
						tmp->parent = currentNode;

						++tokenCursol;
					}
					else{
						// 現在見ている種類が演算子の一覧から見つからなければ、WHERE句は終わります。
						break;
					}
				}

				// 木を根に向かってさかのぼり、根のノードを設定します。
				whereTopNode = currentNode;
				while (whereTopNode->parent){
					whereTopNode = whereTopNode->parent;
				}
			}
		}

		// FROM句を読み込みます。
		if (tokenCursol->kind == TokenKind::FROM){
			++tokenCursol;
		}
		else{
			throw ResultValue::ERR_SQL_SYNTAX;
		}

		first = true; // FROM句の最初のテーブル名を読み込み中かどうかです。
		while (tokenCursol != tokens.end() && tokenCursol->kind == TokenKind::COMMA || first){
			if (tokenCursol->kind == TokenKind::COMMA){
				++tokenCursol;
			}
			if (tokenCursol->kind == TokenKind::IDENTIFIER){
				tableNames.push_back(tokenCursol->word);
				++tokenCursol;
			}
			else{
				throw ResultValue::ERR_SQL_SYNTAX;
			}
			first = false;
		}

		// 最後のトークンまで読み込みが進んでいなかったらエラーです。
		if (tokenCursol != tokens.end()) {
			throw ResultValue::ERR_SQL_SYNTAX;
		}

		vector<vector<Column>> inputColumns;

		for (size_t i = 0; i < tableNames.size(); ++i){

			// 入力ファイル名を生成します。
			const string csvExtension = ".csv"; // csvの拡張子です。
			const string fileName = tableNames[i] + csvExtension; // 拡張子を含む、入力ファイルのファイル名です。

			// 入力ファイルを開きます。
			inputTableFiles.push_back(ifstream(fileName));
			if (!inputTableFiles.back()) {
				throw ResultValue::ERR_FILE_OPEN;
			}

			// 入力CSVのヘッダ行を読み込みます。
			inputColumns.push_back(vector<Column>());
			string inputLine; // ファイルから読み込んだ行文字列です。
			if (getline(inputTableFiles.back(), inputLine)) {
				auto charactorCursol = inputLine.begin();
				auto lineEnd = inputLine.end();

				// 読み込んだ行を最後まで読みます。
				while (charactorCursol != lineEnd){
					// 列名を一つ読みます。
					auto columnStart = charactorCursol;
					charactorCursol = find(charactorCursol, lineEnd, ',');
					inputColumns[i].push_back(Column(tableNames[i], string(columnStart, charactorCursol)));
					// 入力行のカンマの分を読み進めます。
					if (charactorCursol != lineEnd) {
						++charactorCursol;
					}
				}
			}
			else{
				throw ResultValue::ERR_CSV_SYNTAX;
			}

			// 入力CSVのデータ行を読み込みます。
			inputData.push_back(vector<vector<Data>>());

			while (getline(inputTableFiles.back(), inputLine)) {
				inputData[i].push_back(vector<Data>()); // 入力されている一行分のデータです。
				vector<Data> &row = inputData[i].back();

				auto charactorCursol = inputLine.begin(); // データ入力行を検索するカーソルです。
				auto lineEnd = inputLine.end(); // データ入力行のendを指します。
				
				// 読み込んだ行を最後まで読みます。
				while (charactorCursol != lineEnd){
					auto columnStart  = charactorCursol; // 現在の列の最初を記録しておきます。
					charactorCursol = find(charactorCursol, lineEnd, ',');
					
					row.push_back(Data(string(columnStart, charactorCursol)));

					// 入力行のカンマの分を読み進めます。
					if (charactorCursol != lineEnd) {
						++charactorCursol;
					}
				}
			}

			// 全てが数値となる列は数値列に変換します。
			for (size_t j = 0; j <inputColumns[i].size(); ++j) {

				// 全ての行のある列について、データ文字列から符号と数値以外の文字を探します。

				// 符号と数字以外が見つからない列については、数値列に変換します。
				// none_of：無該当の時に真を返す。
				if (none_of(inputData[i].begin(), inputData[i].end(),
					[&](const vector<Data> &inputRow) {
						// any_of：条件式に部分一致すると真を返す。
						return any_of(inputRow[j].string().begin(), inputRow[j].string().end(),
							[&](const char& c) { return signNum.find(c) == string::npos; });
					})) {

					// 符号と数字以外が見つからない列については、数値列に変換します。
					for (auto& inputRow : inputData[i]) {
						inputRow[j] = Data(stoi(inputRow[j].string()));
					}
				}
			}
		}

		vector<Column> allInputColumns; // 入力に含まれるすべての列の一覧です。

		// 入力ファイルに書いてあったすべての列をallInputColumnsに設定します。
		for (size_t i = 0; i < tableNames.size(); ++i){
			for (auto &inputColumn : inputColumns[i]) {
				allInputColumns.push_back(Column(tableNames[i], inputColumn.columnName));
			}
		}

		// SELECT句の列名指定が*だった場合は、入力CSVの列名がすべて選択されます。
		if (selectColumns.empty()){
			for (auto &inputColumn : allInputColumns) {
				selectColumns.push_back(inputColumn);
			}
		}

		vector<Column> outputColumns;

		// SELECT句で指定された列名が、何個目の入力ファイルの何列目に相当するかを判別します。
		vector<ColumnIndex> selectColumnIndexes; // SELECT句で指定された列の、入力ファイルとしてのインデックスです。
		for (auto &selectColumn : selectColumns) {
			found = false;
			for (size_t i = 0; i < tableNames.size(); ++i){
				int j = 0;
				for (auto &inputColumn : inputColumns[i]) {
					if (Equali(selectColumn.columnName, inputColumn.columnName) &&
						(selectColumn.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
						//!*selectTableNameCursol && !*inputTableNameCursol)){
						Equali(selectColumn.tableName, inputColumn.tableName))) {

						// 既に見つかっているのにもう一つ見つかったらエラーです。
						if (found){
							throw ResultValue::ERR_BAD_COLUMN_NAME;
						}
						found = true;
						// 見つかった値を持つ列のデータを生成します。
						selectColumnIndexes.push_back(ColumnIndex(i, j));
					}
					++j;
				}
			}

			// 一つも見つからなくてもエラーです。
			if (!found){
				throw ResultValue::ERR_BAD_COLUMN_NAME;
			}
		}

		// 出力する列名を設定します。
		for (size_t i = 0; i < selectColumns.size(); ++i){
			outputColumns.push_back(inputColumns[selectColumnIndexes[i].table][selectColumnIndexes[i].column]);
		}

		if (whereTopNode){
			// 既存数値の符号を計算します。
			for (auto &whereExtensionNode : whereExtensionNodes) {
				if (whereExtensionNode->middleOperator.kind == TokenKind::NOT_TOKEN &&
					whereExtensionNode->column.columnName.empty() &&
					whereExtensionNode->value.type == DataType::INTEGER) {
					whereExtensionNode->value = Data(whereExtensionNode->value.integer() * whereExtensionNode->signCoefficient);
				}
			}
		}

		for (size_t i = 0; i < tableNames.size(); ++i){
			// 各テーブルの先頭行を設定します。
			currentRows.push_back(inputData[i].begin());
		}

		// 出力するデータを設定します。
		while (true){
			outputData.push_back(vector<Data>());
			vector<Data> &row = outputData.back(); // 出力している一行分のデータです。

			// 行の各列のデータを入力から持ってきて設定します。
			for (size_t i = 0; i < selectColumnIndexes.size(); ++i){
				row.push_back((*currentRows[selectColumnIndexes[i].table])[selectColumnIndexes[i].column]);
			}

			allColumnOutputData.push_back(vector<Data>());
			vector<Data> &allColumnsRow = allColumnOutputData.back();// WHEREやORDERのためにすべての情報を含む行。rowとインデックスを共有します。
			// allColumnsRowの列を設定します。
			for (size_t i = 0; i < tableNames.size(); ++i){
				for (size_t j = 0; j < inputColumns[i].size(); ++j) {
					allColumnsRow.push_back((*currentRows[i])[j]);
				}
			}
			// WHEREの条件となる値を再帰的に計算します。
			if (whereTopNode){
				shared_ptr<ExtensionTreeNode> currentNode = whereTopNode; // 現在見ているノードです。
				while (currentNode){
					// 子ノードの計算が終わってない場合は、まずそちらの計算を行います。
					if (currentNode->left && !currentNode->left->calculated){
						currentNode = currentNode->left;
						continue;
					}
					else if (currentNode->right && !currentNode->right->calculated){
						currentNode = currentNode->right;
						continue;
					}

					// 自ノードの値を計算します。
					switch (currentNode->middleOperator.kind){
					case TokenKind::NOT_TOKEN:
						// ノードにデータが設定されている場合です。

						// データが列名で指定されている場合、今扱っている行のデータを設定します。
						if (!currentNode->column.columnName.empty()){
							found = false;
							for (size_t i = 0; i < allInputColumns.size(); ++i){
								if (Equali(currentNode->column.columnName, allInputColumns[i].columnName) &&
									(currentNode->column.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
									//!*whereTableNameCursol && !*allInputTableNameCursol)){
									Equali(currentNode->column.tableName, allInputColumns[i].tableName))) {
									// 既に見つかっているのにもう一つ見つかったらエラーです。
									if (found){
										throw ResultValue::ERR_BAD_COLUMN_NAME;
									}
									found = true;
									currentNode->value = allColumnsRow[i];
								}
							}
							// 一つも見つからなくてもエラーです。
							if (!found){
								throw ResultValue::ERR_BAD_COLUMN_NAME;
							}
							;
							// 符号を考慮して値を計算します。
							if (currentNode->value.type == DataType::INTEGER){
								currentNode->value = Data(currentNode->value.integer() * currentNode->signCoefficient);
							}
						}
						break;
					case TokenKind::EQUAL:
					case TokenKind::GREATER_THAN:
					case TokenKind::GREATER_THAN_OR_EQUAL:
					case TokenKind::LESS_THAN:
					case TokenKind::LESS_THAN_OR_EQUAL:
					case TokenKind::NOT_EQUAL:
						// 比較演算子の場合です。

						// 比較できるのは文字列型か整数型で、かつ左右の型が同じ場合です。
						if (currentNode->left->value.type != DataType::INTEGER && currentNode->left->value.type != DataType::STRING ||
							currentNode->left->value.type != currentNode->right->value.type){
							throw ResultValue::ERR_WHERE_OPERAND_TYPE;
						}
						currentNode->value.type = DataType::BOOLEAN;

						// 比較結果を型と演算子によって計算方法を変えて、計算します。
						switch (currentNode->left->value.type){
						case DataType::INTEGER:
							switch (currentNode->middleOperator.kind){
							case TokenKind::EQUAL:
								currentNode->value = Data(currentNode->left->value.integer() == currentNode->right->value.integer());
								break;
							case TokenKind::GREATER_THAN:
								currentNode->value = Data(currentNode->left->value.integer() > currentNode->right->value.integer());
								break;
							case TokenKind::GREATER_THAN_OR_EQUAL:
								currentNode->value = Data(currentNode->left->value.integer() >= currentNode->right->value.integer());
								break;
							case TokenKind::LESS_THAN:
								currentNode->value = Data(currentNode->left->value.integer() < currentNode->right->value.integer());
								break;
							case TokenKind::LESS_THAN_OR_EQUAL:
								currentNode->value = Data(currentNode->left->value.integer() <= currentNode->right->value.integer());
								break;
							case TokenKind::NOT_EQUAL:
								currentNode->value = Data(currentNode->left->value.integer() != currentNode->right->value.integer());
								break;
							}
							break;
						case DataType::STRING:
							switch (currentNode->middleOperator.kind){
							case TokenKind::EQUAL:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) == 0);
								break;
							case TokenKind::GREATER_THAN:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) > 0);
								break;
							case TokenKind::GREATER_THAN_OR_EQUAL:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) >= 0);
								break;
							case TokenKind::LESS_THAN:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) < 0);
								break;
							case TokenKind::LESS_THAN_OR_EQUAL:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) <= 0);
								break;
							case TokenKind::NOT_EQUAL:
								currentNode->value = Data(strcmp(currentNode->left->value.string().c_str(), currentNode->right->value.string().c_str()) != 0);
								break;
							}
							break;
						}
						break;
					case TokenKind::PLUS:
					case TokenKind::MINUS:
					case TokenKind::ASTERISK:
					case TokenKind::SLASH:
						// 四則演算の場合です。

						// 演算できるのは整数型同士の場合のみです。
						if (currentNode->left->value.type != DataType::INTEGER || currentNode->right->value.type != DataType::INTEGER){
							throw ResultValue::ERR_WHERE_OPERAND_TYPE;
						}
						currentNode->value.type = DataType::INTEGER;

						// 比較結果を演算子によって計算方法を変えて、計算します。
						switch (currentNode->middleOperator.kind){
						case TokenKind::PLUS:
							currentNode->value = Data(currentNode->left->value.integer() + currentNode->right->value.integer());
							break;
						case TokenKind::MINUS:
							currentNode->value = Data(currentNode->left->value.integer() - currentNode->right->value.integer());
							break;
						case TokenKind::ASTERISK:
							currentNode->value = Data(currentNode->left->value.integer() * currentNode->right->value.integer());
							break;
						case TokenKind::SLASH:
							currentNode->value = Data(currentNode->left->value.integer() / currentNode->right->value.integer());
							break;
						}
						break;
					case TokenKind::AND:
					case TokenKind::OR:
						// 論理演算の場合です。

						// 演算できるのは真偽値型同士の場合のみです。
						if (currentNode->left->value.type != DataType::BOOLEAN || currentNode->right->value.type != DataType::BOOLEAN){
							throw ResultValue::ERR_WHERE_OPERAND_TYPE;
						}
						currentNode->value.type = DataType::BOOLEAN;

						// 比較結果を演算子によって計算方法を変えて、計算します。
						switch (currentNode->middleOperator.kind){
						case TokenKind::AND:
							currentNode->value = Data(currentNode->left->value.boolean() && currentNode->right->value.boolean());
							break;
						case TokenKind::OR:
							currentNode->value = Data(currentNode->left->value.boolean() || currentNode->right->value.boolean());
							break;
						}
					}
					currentNode->calculated = true;

					// 自身の計算が終わった後は親の計算に戻ります。
					currentNode = currentNode->parent;
				}

				// 条件に合わない行は出力から削除します。
				if (!whereTopNode->value.boolean()){
					allColumnOutputData.pop_back();
					outputData.pop_back();
				}
				// WHERE条件の計算結果をリセットします。
				for (auto &whereExtensionNode : whereExtensionNodes) {
					whereExtensionNode->calculated = false;
				}
			}

			// 各テーブルの行のすべての組み合わせを出力します。

			// 最後のテーブルのカレント行をインクリメントします。
			++currentRows[tableNames.size() - 1];

			// 最後のテーブルが最終行になっていた場合は先頭に戻し、順に前のテーブルのカレント行をインクリメントします。
			for (int i = tableNames.size() - 1; currentRows[i] == inputData[i].end() && 0 < i; --i){
				++currentRows[i - 1];
				currentRows[i] = inputData[i].begin();
			}

			// 最初のテーブルが最後の行を超えたなら出力行の生成は終わりです。
			if (currentRows[0] == inputData[0].end()) {
				break;
			}
		}

		// ORDER句による並び替えの処理を行います。
		if (!orderByColumns.empty()){
			// ORDER句で指定されている列が、全ての入力行の中のどの行なのかを計算します。
			vector<int> orderByColumnIndexes; // ORDER句で指定された列の、すべての行の中でのインデックスです。

			for (auto &orderByColumn : orderByColumns) {
				found = false;
				for (size_t i = 0; i < allInputColumns.size(); ++i){
					if (Equali(orderByColumn.columnName, allInputColumns[i].columnName) &&
						(orderByColumn.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
						//!*orderByTableNameCursol && !*allInputTableNameCursol)){
						Equali(orderByColumn.tableName, allInputColumns[i].tableName))) {
						// 既に見つかっているのにもう一つ見つかったらエラーです。
						if (found){
							throw ResultValue::ERR_BAD_COLUMN_NAME;
						}
						found = true;
						orderByColumnIndexes.push_back(i);
					}
				}
				// 一つも見つからなくてもエラーです。
				if (!found){
					throw ResultValue::ERR_BAD_COLUMN_NAME;
				}
			}

			// outputDataとallColumnOutputDataのソートを一緒に行います。簡便のため凝ったソートは使わず、選択ソートを利用します。
			for (size_t i = 0; i < outputData.size(); ++i){
				int minIndex = i; // 現在までで最小の行のインデックスです。
				for (size_t j = i + 1; j < outputData.size(); ++j){
					bool jLessThanMin = false; // インデックスがjの値が、minIndexの値より小さいかどうかです。
					for (size_t k = 0; k < orderByColumnIndexes.size(); ++k){
						const Data &mData = allColumnOutputData[minIndex][orderByColumnIndexes[k]]; // インデックスがminIndexのデータです。
						const Data &jData = allColumnOutputData[j][orderByColumnIndexes[k]]; // インデックスがjのデータです。
						int cmp = 0; // 比較結果です。等しければ0、インデックスjの行が大きければプラス、インデックスminIndexの行が大きければマイナスとなります。
						switch (mData.type)
						{
						case DataType::INTEGER:
							cmp = jData.integer() - mData.integer();
							break;
						case DataType::STRING:
							cmp = strcmp(jData.string().c_str(), mData.string().c_str());
							break;
						}

						// 降順ならcmpの大小を入れ替えます。
						if (orders[k] == TokenKind::DESC){
							cmp *= -1;
						}
						if (cmp < 0){
							jLessThanMin = true;
							break;
						}
						else if (0 < cmp){
							break;
						}
					}
					if (jLessThanMin){
						minIndex = j;
					}
				}
				vector<Data> tmp = outputData[minIndex];
				outputData[minIndex] = outputData[i];
				outputData[i] = tmp;

				tmp = allColumnOutputData[minIndex];
				allColumnOutputData[minIndex] = allColumnOutputData[i];
				allColumnOutputData[i] = tmp;
			}
		}

		// 出力ファイルを開きます。
		outputFile = ofstream(outputFileName);
		if (outputFile.bad()){
			throw ResultValue::ERR_FILE_OPEN;
		}

		// 出力ファイルに列名を出力します。
		for (size_t i = 0; i < selectColumns.size(); ++i){
			outputFile << outputColumns[i].columnName;
			if (i < selectColumns.size() - 1){
				outputFile << ",";
			}
			else{
				outputFile << "\n";
			}
		}

		// 出力ファイルにデータを出力します。
		for (auto& outputRow : outputData) {
			Data* column = &outputRow[0];
			for (size_t i = 0; i < selectColumns.size(); ++i){
				switch (column->type){
				case DataType::INTEGER:
					outputFile << column->integer();
					break;
				case DataType::STRING:
					outputFile << column->string();
					break;
				}

				if (i < selectColumns.size() - 1){
					outputFile << ",";
				}
				else{
					outputFile << "\n";
				}
				++column;
			}
		}
		if (outputFile.bad()){
			throw ResultValue::ERR_FILE_WRITE;
		}

		// 正常時の後処理です。

		// ファイルリソースを解放します。
		for (auto &inputTableFile : inputTableFiles) {
			if (inputTableFile) {
				inputTableFile.close();
				if (inputTableFile.bad()) {
					throw ResultValue::ERR_FILE_CLOSE;
				}
			}
		}
		if (outputFile){
			outputFile.close();
			if (outputFile.bad()){
				throw ResultValue::ERR_FILE_CLOSE;
			}
		}

		return static_cast<int>(ResultValue::OK);
	}
	catch (ResultValue error) {
		return  static_cast<int>(error);
	}
}
