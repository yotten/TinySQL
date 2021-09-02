//! @file

#include "sqlQuery.hpp"
#include "ExecuteSQL.hpp"
#include "resultValue.hpp"
//#pragma warning(disable:4996)

using namespace std;

//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
//! @param [in] sql 実行するSQLです。
//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
//! @return 実行した結果の状態です。
int ExecuteSQL(const string, const string);

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
//! 参照するテーブルは、テーブル名.csvの形で作成します。
//! 一行目はヘッダ行で、その行に列名を書きます。
//! 前後のスペース読み飛ばしやダブルクォーテーションでくくるなどの機能はありません。
//! 列の型の定義はできないので、列のすべてのデータの値が数値として解釈できる列のデータを整数として扱います。
//! 実行するSQLで使える機能を以下に例としてあげます。
//! 例1:
//! SELECT *
//! FROM USERS
//!
//! 例2: 大文字小文字は区別しません。
//! select *
//! from users
//!
//! 例3: 列の指定ができます。
//! SELECT Id, Name
//! FROM USERS
//!
//! 例4: テーブル名を指定して列の指定ができます。
//! SELECT USERS.Id
//! FROM USERS
//!
//! 例5: ORDER句が使えます。
//! SELECT *
//! ORDER BY NAME
//! FROM USERS
//!
//! 例6: ORDER句に複数列や昇順、降順の指定ができます。
//! SELECT *
//! ORDER BY AGE DESC, Name ASC
//!
//! 例7: WHERE句が使えます。
//! SELECT *
//! WHERE AGE >= 20
//! FROM USERS
//!
//! 例8: WHERE句では文字列の比較も使えます。
//! SELECT *
//! WHERE NAME >= 'N'
//! FROM USERS
//!
//! 例9: WHERE句には四則演算、カッコ、AND、ORなどを含む複雑な式が利用できます。
//! SELECT *
//! WHERE AGE >= 20 AND (AGE <= 40 || WEIGHT < 100)
//! FROM USERS
//!
//! 例10: FROM句に複数のテーブルが指定できます。その場合はクロスで結合します。
//! SELECT *
//! FROM USERS, CHILDREN
//!
//! 例11: WHEREで条件をつけることにより、テーブルの結合ができます。
//! SELECT USERS.NAME, CHILDREN.NAME
//! WHERE USERS.ID = CHILDREN.PARENTID
//! FROM USERS, CHILDREN
int ExecuteSQL(const string sql, const string outputFileName)
{
	try {
		SqlQuery(sql).Execute(outputFileName);
		return static_cast<int>(ResultValue::OK);
	}
	catch (ResultValue error) {
		return static_cast<int>(error);
	}
}