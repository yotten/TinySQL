#include "token.hpp"
#include "data.hpp"
#include "operator.hpp"
#include "sqlQueryInfo.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>

//! ファイルに対して実行するSQLを表すクラスです。
class SqlQuery {
	std::ofstream outputFile;										//!< 書き込むファイルのファイルポインタです。
	std::vector<std::vector<Data>> outputData;							//!< 出力データです。
	std::vector<std::vector<Data>> allColumnOutputData;					//!< 出力するデータに対応するインデックスを持ち、すべての入力データを保管します。

	const std::string alpahUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ"; //!< 全てのアルファベットの大文字小文字とアンダーバーです。
	const std::string alpahNumUnder = "_abcdefghijklmnopqrstuvwxzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; //!< 全ての数字とアルファベットの大文字小文字とアンダーバーです。
	const std::string signNum = "+-0123456789"; //!< 全ての符号と数字です。
	const std::string num = "0123456789"; //!< 全ての数字です。
	const std::string space = " \t\r\n"; //!< 全ての空白文字です。
	
	const std::vector<Token> keywordConditions; //!< キーワードをトークンとして認識するためのキーワード一覧情報です。
	const std::vector<Token> signConditions;    //!< 記号をトークンとして認識するための記号一覧情報です。
	const std::vector<Operator> operators;      //!< 演算子の情報です。

    std::vector<std::vector<Column>> inputColumns;

    std::string m_outputFileName;   //!< outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
    
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
    const std::shared_ptr<std::vector<std::vector<std::vector<Data>>>> ReadCsv(const SqlQueryInfo& queryInfo);
    //! CSVファイルに出力データを書き込みます。
    //! @param [in] queryInfo SQLの情報です。
	//! @param [in] inputData ファイルから読み取ったデータです。
	void WriteCsv(const SqlQueryInfo& queryInfo, std::vector<std::vector<std::vector<Data>>> &inputData);
public:
	//! SqlQueryクラスの新しいインスタンスを初期化します。
	SqlQuery();
	//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
	//! @param [in] sql 実行するSQLです。
	//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
	//! @return 実行した結果の状態です。
	int Execute(const std::string sql, const std::string outputFileName);
};