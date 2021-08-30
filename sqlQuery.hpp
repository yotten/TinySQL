#include "token.hpp"
#include "data.hpp"
#include "operator.hpp"
#include "token.hpp"
#include "extension_tree_node.hpp"
#include "column_index.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstring>

//! ファイルに対して実行するSQLを表すクラスです。
class SqlQuery {
	std::vector<std::ifstream> inputTableFiles;							//!< 読み込むファイルの全ての入力ストリーム
	std::ofstream outputFile;										//!< 書き込むファイルのファイルポインタです。
	bool found = false;                                     //!< 検索時に見つかったかどうかの結果を一時的に保存します。
	std::vector<std::vector<std::vector<Data>>> inputData;						//!< 入力データです。
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

	std::vector<Token> tokens; //!< SQLを分割したトークンです。
	std::vector<std::string> tableNames; //!< FROM句で指定しているテーブル名です。

	std::vector<TokenKind> orders;       //!< 同じインデックスのorderByColumnsに対応している、昇順、降順の指定です。
	std::shared_ptr<ExtensionTreeNode> whereTopNode; // 式木の根となるノードです。
    std::vector<Column> selectColumns; // SELECT句に指定された列名です。
	std::vector<std::shared_ptr<ExtensionTreeNode>> whereExtensionNodes;
	std::vector<Column> orderByColumns; // ORDER句に指定された列名です。
    std::vector<std::vector<Column>> inputColumns;

    std::string m_sql;              //!< 実行するSQLです。
    std::string m_outputFileName;   //!< outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
    
    bool Equali(const std::string str1, const std::string str2);
	void GetTokens();               //! SQLの文字列からトークンを切り出します。
	void AnalyzeTokens();           //! トークンを解析してSQLの構文で指定された情報を取得します。
    void ReadCsv();                 //! CSVファイルから入力データを読み取ります。
	void WriteCsv();                //! CSVファイルに出力データを書き込みます。
public:
	//! SqlQueryクラスの新しいインスタンスを初期化します。
	SqlQuery();
	//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
	//! @param [in] sql 実行するSQLです。
	//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
	//! @return 実行した結果の状態です。
	int Execute(const std::string sql, const std::string outputFileName);
};