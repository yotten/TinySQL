#include "token.hpp"
#include "data.hpp"
#include "operator.hpp"
#include "sqlQueryInfo.hpp"
#include "inputTable.hpp"
#include "tokenReader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>

//! ファイルに対して実行するSQLを表すクラスです。
class SqlQuery {
	const std::string signNum = "+-0123456789"; //!< 全ての符号と数字です。
	const std::string space = " \t\r\n"; //!< 全ての空白文字です。
	
	const std::vector<std::shared_ptr<const TokenReader>> tokenReaders; //!< トークンの読み込みロジックの集合です。
	// signConditionsは先頭から順に検索されるので、前方一致となる二つの項目は順番に気をつけて登録しなくてはいけません。
	const std::vector<Token> signConditions;    //!< 記号をトークンとして認識するための記号一覧情報です。
	const std::vector<Operator> operators;      //!< 演算子の情報です。
    std::shared_ptr<const SqlQueryInfo> queryInfo; //!< SQLに記述された内容です。

    bool Equali(const std::string str1, const std::string str2);
	//! @param [in] sql トークンに分解する元となるSQLです。
	//! @return 切り出されたトークンです。
	const std::shared_ptr<std::vector<Token>> GetTokens(const std::string sql) const;
    //! @param [in] tokens 解析の対象となるトークンです。
    //! @return 解析した結果の情報です。
    const std::shared_ptr<const SqlQueryInfo> AnalyzeTokens(const std::vector<Token> &tokens) const;
    //! CSVファイルから入力データを読み取ります。
    //! @param [in] queryInfo SQLの情報です。
	//! @return ファイルから読み取ったデータです。
    const std::shared_ptr<const std::vector<InputTable>> ReadCsv() const;
    //! CSVファイルに出力データを書き込みます。
    //! @param [in] queryInfo SQLの情報です。
	//! @param [in] inputData ファイルから読み取ったデータです。
	void WriteCsv(const std::string outputFileName, const std::vector<InputTable> &inputTables) const;
public:
	//! SqlQueryクラスの新しいインスタンスを初期化します。
    //! @param [in] sql 実行するSQLです。
	SqlQuery(const std::string sql);
	//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
	//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
	void Execute(const std::string outputFileName);
};