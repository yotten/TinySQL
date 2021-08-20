#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <gtest/gtest.h>

#include "ExecuteSQL.hpp"

//#define TestNo16 DISABLED_TestNo16
//#define TestNo17 DISABLED_TestNo17
#define TestNo19 DISABLED_TestNo19
//#define TestNo30 DISABLED_TestNo30
//#define TestNo58 DISABLED_TestNo58
//#define TestNo65 DISABLED_TestNo65
//#define TestNo66 DISABLED_TestNo66
//#define TestNo67 DISABLED_TestNo67
//#define TestNo68 DISABLED_TestNo68
#define TestNo114 DISABLED_TestNo114
#define TestNo115 DISABLED_TestNo115
//#define TestNo160 DISABLED_TestNo160

//! ExecuteSQLの戻り値の種類を表します。
enum REAULT_VALUE
{
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

using namespace std;

class MyTest : public ::testing::Test {
protected:
    const char *testOutputPath = "output.csv";

    string ReadOutput()
	{
        ifstream ifs(testOutputPath);
        string str;
        string rtn_str;
        while (getline(ifs, str)) {
            rtn_str += str;
            rtn_str += "\n";
        }
        return rtn_str;
	}

    virtual void SetUp() {
        ofstream o;
        remove(testOutputPath);
        o = ofstream("TABLE1.csv");
        o
            << "Integer,String" << endl
            << "1,A" << endl
            << "2,B" << endl
            << "3,C" << endl;
        o = ofstream("TABLE2.csv");
        o
            << "Integer,String" << endl
            << "4,D" << endl
            << "5,E" << endl
            << "6,F" << endl;
        o = ofstream("TABLE3.csv");
        o
            << "Integer,String" << endl
            << "7,G" << endl
            << "8,H" << endl;
        o = ofstream("UNORDERED.csv");
        o
            << "Integer,String" << endl
            << "21,BA" << endl
            << "2,B" << endl
            << "12,AB" << endl
            << "11,AA" << endl
            << "22,BB" << endl
            << "1,A" << endl;
        o = ofstream("UNORDERED2.csv");
        o
            << "Integer1,Integer2,String1,String2" << endl
            << "1,2,A,B" << endl
            << "2,2,B,B" << endl
            << "2,1,B,A" << endl
            << "1,1,A,A" << endl;
        o = ofstream("PARENTS.csv");
        o
            << "Id,Name" << endl
            << "1,Parent1" << endl
            << "2,Parent2" << endl
            << "3,Parent3" << endl;
        o = ofstream("CHILDREN.csv");
        o
            << "Id,Name,ParentId" << endl
            << "1,Child1,1" << endl
            << "2,Child2,1" << endl
            << "3,Child3,2" << endl
            << "4,Child4,2" << endl
            << "5,Child5,3" << endl
            << "6,Child6,3" << endl
            << "7,Child7,3" << endl;
        o = ofstream("MINUS.csv");
        o
            << "Integer" << endl
            << "-1" << endl
            << "-2" << endl
            << "-3" << endl
            << "-4" << endl
            << "-5" << endl
            << "-6" << endl;
    };

    virtual void TearDown() {
        
    };
};

TEST_F(MyTest, TestNo1) {//ExecuteSQLは単純なSQLを実行できます。
    const string sql =
        "SELECT * "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result) << "ExecuteSQLは単純なSQLを実行できます。"; 
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo2) { //ExecuteSQLは最後に空白があっても正しく動作します。
    const string sql =
        "SELECT * "
        "FROM TABLE1 ";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo3) { //ExecuteSQLは識別子名に数字を利用できます。
    const string sql =
        "SELECT * "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
}
TEST_F(MyTest, TestNo4) { //ExecuteSQLは識別子名に数字で始まる単語は利用できません。
    const string sql =
        "SELECT * "
        "FROM 1TABLE";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_TOKEN_CANT_READ, result);
}
TEST_F(MyTest, TestNo5) { //ExecuteSQLは識別子名の２文字目に数字を利用できます。)
    const string sql =
        "SELECT * "
        "FROM T1ABLE";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_FILE_OPEN, result);
}
TEST_F(MyTest, TestNo6) { //ExecuteSQLは識別子名の先頭にアンダーバーを利用できます。)
    const string sql =
        "SELECT * "
        "FROM _TABLE";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_FILE_OPEN, result);
}
TEST_F(MyTest, TestNo7) { //ExecuteSQLは識別子名の二文字目以降にアンダーバーを利用できます。)
    const string sql =
        "SELECT * "
        "FROM T_ABLE";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_FILE_OPEN, result);
}
TEST_F(MyTest, TestNo8) { //ExecuteSQLは複数個続く区切り文字を利用できます。)
    const string sql =
        "SELECT  *  "
        "FROM  TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo9) { //ExecuteSQLは区切り文字としてスペースを認識します。)
    const string sql =
        "SELECT * "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo10) { //ExecuteSQLは区切り文字としてタブを認識します。)
    const string sql =
        "SELECT\t*\t"
        "FROM\tTABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo11) { //ExecuteSQLは区切り文字として改行を認識します。)
    const string sql =
        "SELECT\n*\r\n"
        "FROM\rTABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo12) { //ExecuteSQLは認識できないトークンを含む語を指定したときERR_TOKEN_CANT_READエラーとなります。)
    const string sql =
        "?";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_TOKEN_CANT_READ, result);
}
TEST_F(MyTest, TestNo13) { //ExecuteSQLは指定したテーブル名を取得し、対応するファイルを参照できます。)
    const string sql =
        "SELECT * "
        "FROM TABLE2";

    string expectedCsv =
        "Integer,String"	"\n"
        "4,D"				"\n"
        "5,E"				"\n"
        "6,F"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo14) { //ExecuteSQLは二つののテーブルを読み込み、全ての組み合わせを出力します。)
    const string sql =
        "SELECT * "
        "FROM TABLE1, TABLE2";

    string expectedCsv =
        "Integer,String,Integer,String"	"\n"
        "1,A,4,D"						"\n"
        "1,A,5,E"						"\n"
        "1,A,6,F"						"\n"
        "2,B,4,D"						"\n"
        "2,B,5,E"						"\n"
        "2,B,6,F"						"\n"
        "3,C,4,D"						"\n"
        "3,C,5,E"						"\n"
        "3,C,6,F"						"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo15) { //ExecuteSQLは三つ以上のテーブルを読み込み、全ての組み合わせを出力します。)
    const string sql =
        "SELECT * "
        "FROM TABLE1, TABLE2, TABLE3";

    string expectedCsv =
        "Integer,String,Integer,String,Integer,String"	"\n"
        "1,A,4,D,7,G"									"\n"
        "1,A,4,D,8,H"									"\n"
        "1,A,5,E,7,G"									"\n"
        "1,A,5,E,8,H"									"\n"
        "1,A,6,F,7,G"									"\n"
        "1,A,6,F,8,H"									"\n"
        "2,B,4,D,7,G"									"\n"
        "2,B,4,D,8,H"									"\n"
        "2,B,5,E,7,G"									"\n"
        "2,B,5,E,8,H"									"\n"
        "2,B,6,F,7,G"									"\n"
        "2,B,6,F,8,H"									"\n"
        "3,C,4,D,7,G"									"\n"
        "3,C,4,D,8,H"									"\n"
        "3,C,5,E,7,G"									"\n"
        "3,C,5,E,8,H"									"\n"
        "3,C,6,F,7,G"									"\n"
        "3,C,6,F,8,H"									"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo16) { //ExecuteSQLはSQLECT句にテーブルと一緒に指定した列名を指定し、SQLを実行できます。)
    const string sql =
        "SELECT String "
        "FROM TABLE1";

    string expectedCsv =
        "String"	"\n"
        "A"			"\n"
        "B"			"\n"
        "C"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo17) { //ExecuteSQLはSQLECT句に複数のテーブルと一緒に指定した列名を指定し、SQLを実行できます。)
    const string sql =
        "SELECT String,Integer "
        "FROM TABLE1";

    string expectedCsv =
        "String,Integer"	"\n"
        "A,1"				"\n"
        "B,2"				"\n"
        "C,3"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo18) { //ExecuteSQLはSQLECT句に三つ以上のテーブルと一緒に指定した列名を指定し、SQLを実行できます。)
    const string sql =
        "SELECT String,Integer,String,Integer "
        "FROM TABLE1";

    string expectedCsv =
        "String,Integer,String,Integer"	"\n"
        "A,1,A,1"						"\n"
        "B,2,B,2"						"\n"
        "C,3,C,3"						"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo19) { //ExecuteSQLはSELECTの指定にテーブル名も指定できます。)
    const string sql =
        "SELECT TABLE1.Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer"	"\n"
        "1"			"\n"
        "2"			"\n"
        "3"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo20) { //ExecuteSQLは複数ののテーブルを読み込み、テーブル名で区別してテーブルと一緒に指定した列名を指定することができます。)
    const string sql =
        "SELECT Table1.Integer "
        "FROM TABLE1, TABLE2";

    string expectedCsv =
        "Integer"	"\n"
        "1"			"\n"
        "1"			"\n"
        "1"			"\n"
        "2"			"\n"
        "2"			"\n"
        "2"			"\n"
        "3"			"\n"
        "3"			"\n"
        "3"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo21) { //ExecuteSQLはSELECT句でテーブル名を二つ目以降のテーブルと一緒に指定した列名に指定することができます。)
    const string sql =
        "SELECT Table1.Integer, Table2.String "
        "FROM TABLE1, TABLE2";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,D"				"\n"
        "1,E"				"\n"
        "1,F"				"\n"
        "2,D"				"\n"
        "2,E"				"\n"
        "2,F"				"\n"
        "3,D"				"\n"
        "3,E"				"\n"
        "3,F"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo22) { //ExecuteSQLはSELECTのテーブルと一緒に指定した列名の指定があいまいな場合にERR_BAD_COLUMN_NAMEエラーとなります。)
    const string sql =
        "SELECT Integer "
        "FROM TABLE1, TABLE2";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo23) { //ExecuteSQLはSELECTで指定したテーブルと一緒に指定した列名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT Ttring "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo24) { //ExecuteSQLはSELECTで指定したテーブルと一緒に指定した列名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT Suring "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo25) { //ExecuteSQLはSELECTで指定したテーブルと一緒に指定した列名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT Surinh "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo26) { //ExecuteSQLはSELECTで指定したテーブルと一緒に指定した列名の指定が一文字多いという違いを見分けます。)
    const string sql =
        "SELECT Suringg "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo27) { //ExecuteSQLはSELECTで指定したテーブルと一緒に指定した列名の指定の一文字少ないという違いを見分けます。)
    const string sql =
        "SELECT Surin "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo28) { //ExecuteSQLはSELECTで指定したテーブル名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT UABLE1.Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo29) { //ExecuteSQLはSELECTで指定したテーブル名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT TBBLE1.Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo30) { //ExecuteSQLはSELECTで指定したテーブル名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT TABLE2.Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo31) { //ExecuteSQLはSELECTで指定したテーブル名の指定が一文字多いという違いを見分けます。)
    const string sql =
        "SELECT TABLE1a.Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo32) { //ExecuteSQLはSELECTで指定したテーブル名の指定の一文字少ないという違いを見分けます。)
    const string sql =
        "SELECT TABLE.Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo33) { //ExecuteSQLはORDER句で文字列を辞書順で並べ替えます。)
    const string sql =
        "SELECT * "
        "ORDER BY String "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "2,B"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo34) { //ExecuteSQLはORDER句にSELECTで指定されなかった列を指定することができます。)
    const string sql =
        "SELECT String "
        "ORDER BY Integer "
        "FROM UNORDERED";

    string expectedCsv =
        "String"	"\n"
        "A"			"\n"
        "B"			"\n"
        "AA"		"\n"
        "AB"		"\n"
        "BA"		"\n"
        "BB"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo35) { //ExecuteSQLはORDER句にSELECTで指定されなかった、入力の最後の列を指定することができます。)
    const string sql =
        "SELECT Integer "
        "ORDER BY String "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer"	"\n"
        "1"			"\n"
        "11"		"\n"
        "12"		"\n"
        "2"			"\n"
        "21"		"\n"
        "22"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo36) { //ExecuteSQLはORDER句で数字列を大小順で並べ替えます。)
    const string sql =
        "SELECT * "
        "ORDER BY Integer "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo37) { //ExecuteSQLはORDER句でマイナスの数値を扱えます。)
    const string sql =
        "SELECT * "
        "ORDER BY Integer "
        "FROM MINUS";

    string expectedCsv =
        "Integer"	"\n"
        "-6"		"\n"
        "-5"		"\n"
        "-4"		"\n"
        "-3"		"\n"
        "-2"		"\n"
        "-1"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo38) { //ExecuteSQLはORDER句で複数の文字列を条件にして並べ替えます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1, String2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,A"				"\n"
        "A,B"				"\n"
        "B,A"				"\n"
        "B,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo39) { //ExecuteSQLはORDER句で複数の数値列を条件にして並べ替えます。)
    const string sql =
        "SELECT Integer1, Integer2 "
        "ORDER BY Integer1, Integer2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "Integer1,Integer2"	"\n"
        "1,1"				"\n"
        "1,2"				"\n"
        "2,1"				"\n"
        "2,2"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo40) { //ExecuteSQLはORDER句で複数の条件を指定した場合に先に指定した条件を優先して並べ替えます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String2, String1 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,A"				"\n"
        "B,A"				"\n"
        "A,B"				"\n"
        "B,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo41) { //ExecuteSQLはORDER句で昇順を指定できます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1 ASC, String2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,A"				"\n"
        "A,B"				"\n"
        "B,A"				"\n"
        "B,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo42) { //ExecuteSQLはORDER句で降順を指定できます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1 DESC, String2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "B,A"				"\n"
        "B,B"				"\n"
        "A,A"				"\n"
        "A,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo43) { //ExecuteSQLはORDER句で二つ目以降の項目に昇順を指定できます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1 , String2 ASC "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,A"				"\n"
        "A,B"				"\n"
        "B,A"				"\n"
        "B,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo44) { //ExecuteSQLはORDER句で二つ目以降の項目に降順を指定できます。)
    const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1, String2 DESC "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,B"				"\n"
        "A,A"				"\n"
        "B,B"				"\n"
        "B,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo45) { //ExecuteSQLはORDER句にテーブル名付のテーブルと一緒に指定した列名を指定することができます。)
    const string sql =
        "SELECT String "
        "ORDER BY UNORDERED.String "
        "FROM UNORDERED";

    string expectedCsv =
        "String"	"\n"
        "A"			"\n"
        "AA"		"\n"
        "AB"		"\n"
        "B"			"\n"
        "BA"		"\n"
        "BB"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo46) { //ExecuteSQLはORDER句にテーブル名付のテーブルと一緒に指定した列名を指定し、テーブルを選択することができます。)
    const string sql =
        "SELECT *"
        "ORDER BY Table2.String "
        "FROM TABLE1, TABLE2";

    string expectedCsv =
        "Integer,String,Integer,String"	"\n"
        "1,A,4,D"				"\n"
        "2,B,4,D"				"\n"
        "3,C,4,D"				"\n"
        "1,A,5,E"				"\n"
        "2,B,5,E"				"\n"
        "3,C,5,E"				"\n"
        "1,A,6,F"				"\n"
        "2,B,6,F"				"\n"
        "3,C,6,F"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo47) { //ExecuteSQLはORDERBY指定したテーブルと一緒に指定した列名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY Ttring "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo48) { //ExecuteSQLはORDERBY指定したテーブルと一緒に指定した列名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY Suring "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo49) { //ExecuteSQLはORDERBY指定したテーブルと一緒に指定した列名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY Strinh "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo50) { //ExecuteSQLはORDERBY指定したテーブルと一緒に指定した列名の指定が一文字多いという違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY Stringg "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo51) { //ExecuteSQLはORDERBY指定したテーブルと一緒に指定した列名の指定の一文字少ないという違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY Strin "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo52) { //ExecuteSQLはORDERBY指定したテーブル名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY VNORDERED.String "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo53) { //ExecuteSQLはORDERBY指定したテーブル名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY UMORDERED.String "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo54) { //ExecuteSQLはORDERBY指定したテーブル名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY UNORDEREE.String "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo55) { //ExecuteSQLはORDERBY指定したテーブル名の指定が一文字多いという違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY UNORDEREDD.String "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo56) { //ExecuteSQLはORDERBY指定したテーブル名の指定の一文字少ないという違いを見分けます。)
    const string sql =
        "SELECT * "
        "ORDER BY UNORDERE.String "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo57) { //ExecuteSQLはORDERBYで曖昧なテーブルと一緒に指定した列名を指定した場合にERR_BAD_COLUMN_NAMEエラーとなります。)
    const string sql =
        "SELECT * "
        "ORDER BY String "
        "FROM TABLE1, TABLE2";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo58) { //ExecuteSQLはWHERE句で数値列に対する条件として文字列は指定できません。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'2\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo59) { //ExecuteSQLはWHERE句で数値として等しい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo60) { //ExecuteSQLはWHERE句で数値として等しくない条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer <> 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo61) { //ExecuteSQLはWHERE句で数値として大きい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer > 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo62) { //ExecuteSQLはWHERE句で数値として小さい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer < 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo63) { //ExecuteSQLはWHERE句で数値として以上の条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer >= 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo64) { //ExecuteSQLはWHERE句で数値として以下の条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer <= 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo65) { //ExecuteSQLはWHERE句でマイナスの数値が扱えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer < -3 "
        "FROM MINUS";

    string expectedCsv =
        "Integer"	"\n"
        "-4"		"\n"
        "-5"		"\n"
        "-6"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo66) { //ExecuteSQLはWHERE句でプラスを明示したの数値が扱えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer <= +2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo67) { //ExecuteSQLはWHERE句でマイナスを指定したの列名が扱えます。)
    const string sql =
        "SELECT * "
        "WHERE -Integer > 3 "
        "FROM MINUS";

    string expectedCsv =
        "Integer"	"\n"
        "-4"		"\n"
        "-5"		"\n"
        "-6"		"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo68) { //ExecuteSQLはWHERE句でプラスを明示した列名が扱えます。)
    const string sql =
        "SELECT * "
        "WHERE +Integer <= 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo69) { //ExecuteSQLはWHERE句で文字列にマイナスの指定はできません。)
    const string sql =
        "SELECT * "
        "WHERE String = -\'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo70) { //ExecuteSQLはWHERE句で文字列にプラスの指定はできません。)
    const string sql =
        "SELECT * "
        "WHERE String = +\'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo71) { //ExecuteSQLはWHERE句でSELECT句で指定していない列の条件の指定ができます。)
    const string sql =
        "SELECT String "
        "WHERE Integer = 2 "
        "FROM TABLE1";

    string expectedCsv =
        "String"	"\n"
        "B"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo72) { //ExecuteSQLはWHERE句でSELECT句で指定していない、入力の最後の列がテーブル名を指定せずに条件の指定ができます。)
    const string sql =
        "SELECT Integer "
        "WHERE String = \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer"	"\n"
        "2"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo73) { //ExecuteSQLはWHERE句でSELECT句で指定していない、入力の最後の列がテーブル名を指定して条件の指定ができます。)
    const string sql =
        "SELECT Integer "
        "WHERE TABLE1.String = \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer"	"\n"
        "2"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo74) { //ExecuteSQLはWHERE句で文字列と数値の等しい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo75) { //ExecuteSQLはWHERE句で文字列と数値の等しくない条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String <> 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo76) { //ExecuteSQLはWHERE句で文字列と数値の小さい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String < 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo77) { //ExecuteSQLはWHERE句で文字列と数値の以下条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String <= 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo78) { //ExecuteSQLはWHERE句で文字列と数値の大きい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String > 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo79) { //ExecuteSQLはWHERE句で文字列と数値の以上条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE String >= 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo80) { //ExecuteSQLはWHERE句で数値と文字列の等しい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo81) { //ExecuteSQLはWHERE句で数値と文字列の等しくない条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer <> \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo82) { //ExecuteSQLはWHERE句で数値と文字列の小さい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer < \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo83) { //ExecuteSQLはWHERE句で数値と文字列の以下条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer <= \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo84) { //ExecuteSQLはWHERE句で数値と文字列の大きい条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer > \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo85) { //ExecuteSQLはWHERE句で数値と文字列の以上条件の比較をした場合にERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer >= \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo86) { //ExecuteSQLはWHERE句で文字列として等しい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String = \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo87) { //ExecuteSQLはWHERE句で文字列として等しくない条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String <> \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo88) { //ExecuteSQLはWHERE句で文字列として大きい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String > \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo89) { //ExecuteSQLはWHERE句で文字列として小さい条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String < \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo90) { //ExecuteSQLはWHERE句で文字列として以上の条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String >= \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo91) { //ExecuteSQLはWHERE句で文字列として以下の条件の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE String <= \'B\' "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo92) { //ExecuteSQLはWHERE指定したテーブルと一緒に指定した列名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE Ttring = \'A\' "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo93) { //ExecuteSQLはWHERE指定したテーブルと一緒に指定した列名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE Suring = \'A\' "
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo94) { //ExecuteSQLはWHERE指定したテーブルと一緒に指定した列名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE Strinh  = \'A\'"
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo95) { //ExecuteSQLはWHERE指定したテーブルと一緒に指定した列名の指定が一文字多いという違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE Stringg  = \'A\'"
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo96) { //ExecuteSQLはWHERE指定したテーブルと一緒に指定した列名の指定の一文字少ないという違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE Strin  = \'A\'"
        "FROM UNORDERED";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo97) { //ExecuteSQLはWHERE句で比較のテーブルと一緒に指定した列名を右辺に持ってくることができます。)
    const string sql =
        "SELECT * "
        "WHERE 2 = Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo98) { //ExecuteSQLはWHERE句で加算演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 + 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo99) { //ExecuteSQLはWHERE句の加算演算子の左辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'A\' + 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo100) { //ExecuteSQLはWHERE句の加算演算子の右辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 + \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo101) { //ExecuteSQLはWHERE句で減算演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 3 - 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo102) { //ExecuteSQLはWHERE句の減算演算子の左辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'A\' - 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo103) { //ExecuteSQLはWHERE句の減算演算子の右辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 - \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo104) { //ExecuteSQLはWHERE句で乗算演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 * 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo105) { //ExecuteSQLはWHERE句の乗算演算子の左辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'A\' * 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo106) { //ExecuteSQLはWHERE句の乗算演算子の右辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 * \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo107) { //ExecuteSQLはWHERE句で除算演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 5 / 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo108) { //ExecuteSQLはWHERE句の除算演算子の左辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = \'A\' / 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo109) { //ExecuteSQLはWHERE句の除算演算子の右辺が数値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 / \'B\' "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo110) { //ExecuteSQLはWHERE句でAND演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE 1 < Integer AND Integer < 3 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo111) { //ExecuteSQLはWHERE句のAND演算子の左辺が真偽値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE 2 AND Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo112) { //ExecuteSQLはWHERE句のAND演算子の右辺が真偽値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 AND 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo113) { //ExecuteSQLはWHERE句でOR演算子が使えます。)
    const string sql =
        "SELECT * "
        "WHERE Integer < 2 OR 2 < Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo114) { //ExecuteSQLはWHERE句のOR演算子の左辺が真偽値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE 2 OR Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo115) { //ExecuteSQLはWHERE句のOR演算子の右辺が真偽値でない場合はERR_WHERE_OPERAND_TYPEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 OR 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_WHERE_OPERAND_TYPE, result);
}
TEST_F(MyTest, TestNo116) { //ExecuteSQLはWHERE句で演算子の優先順位が考慮されます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 2 * 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo117) { //ExecuteSQLはWHERE句で加算演算子は減算演算子より強くはない優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 2 - 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo118) { //ExecuteSQLはWHERE句で乗算演算子は減算演算子より強い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 8 - 3 * 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo119) { //ExecuteSQLはWHERE句で乗算演算子は加算演算子より強い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 + 1 * 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo120) { //ExecuteSQLはWHERE句で乗算演算子は除算演算子と同じ優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 2 * 5 / 3 * 2 - 4 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo121) { //ExecuteSQLはWHERE句で等しい演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo122) { //ExecuteSQLはWHERE句で等しくない演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer <> 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo123) { //ExecuteSQLはWHERE句で大きい演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer > 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo124) { //ExecuteSQLはWHERE句で小さい演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer < 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo125) { //ExecuteSQLはWHERE句で以上演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer >= 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo126) { //ExecuteSQLはWHERE句で以下演算子は加算演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer <= 1 + 1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo127) { //ExecuteSQLはWHERE句でAND演算子は比較演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE 1 < Integer AND Integer < 3 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo128) { //ExecuteSQLはWHERE句でOR演算子はAND演算子より弱い優先順位です。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 OR Integer <= 2 AND 2 <= Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo129) { //ExecuteSQLはWHERE句でカッコによる優先順位の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = (1 + 2) * 3 - 7 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo130) { //ExecuteSQLはWHERE句でカッコにより左結合を制御することができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 1 - (2 - 3) "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo131) { //ExecuteSQLはWHERE句でネストしたカッコによる優先順位の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = (2 * (2 + 1) + 2) / 3 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo132) { //ExecuteSQLはWHERE句でカッコ内部の演算子の優先順位の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = (3 * 2 - 2 * 2) "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo133) { //ExecuteSQLはWHERE句でカッコ開くを連続で記述することができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = ((3 - 2) * 2) "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo134) { //ExecuteSQLはWHERE句でカッコ閉じるを連続で記述することができます。)
    const string sql =
        "SELECT * "
        "WHERE Integer = (2 * (3 - 2))"
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo135) { //ExecuteSQLはWHERE句でテーブル名の指定ができます。)
    const string sql =
        "SELECT * "
        "WHERE TABLE1.Integer = 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo136) { //ExecuteSQLはWHERE句にテーブル名付のテーブルと一緒に指定した列名を指定し、テーブルを選択することができます。)
    const string sql =
        "SELECT *"
        "WHERE Table2.Integer = 5 "
        "FROM TABLE1, TABLE2";

    string expectedCsv =
        "Integer,String,Integer,String"	"\n"
        "1,A,5,E"				"\n"
        "2,B,5,E"				"\n"
        "3,C,5,E"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo137) { //ExecuteSQLはWHERE句を利用して結合を行うことができます。)
    const string sql =
        "SELECT PARENTS.Name, CHILDREN.Name "
        "WHERE PARENTS.Id = CHILDREN.ParentId "
        "FROM PARENTS, CHILDREN";

    string expectedCsv =
        "Name,Name"			"\n"
        "Parent1,Child1"	"\n"
        "Parent1,Child2"	"\n"
        "Parent2,Child3"	"\n"
        "Parent2,Child4"	"\n"
        "Parent3,Child5"	"\n"
        "Parent3,Child6"	"\n"
        "Parent3,Child7"	"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo138) { //ExecuteSQLはWHERE句のテーブルと一緒に指定した列名の指定があいまいな場合にERR_BAD_COLUMN_NAMEエラーとなります。)
    const string sql =
        "SELECT * "
        "WHERE Integer = 2 "
        "FROM TABLE1, TABLE2";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo139) { //ExecuteSQLはWHERE句で指定したテーブルと一緒に指定した列名の指定の一文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE TABLE1.Jnteger = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo140) { //ExecuteSQLはWHERE句で指定したテーブルと一緒に指定した列名の指定の二文字目の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE TABLE1.Ioteger = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo141) { //ExecuteSQLはWHERE句で指定したテーブルと一緒に指定した列名の指定の最終文字の違いを見分けます。)
    const string sql =
        "SELECT * "
        "WHERE TABLE1.Integes = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo142) { //ExecuteSQLはWHERE句で指定したテーブルと一緒に指定した列名の指定が一文字多いという違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TABLE1.Integerr = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo143) { //ExecuteSQLはWHERE句で指定したテーブルと一緒に指定した列名の指定の一文字少ないという違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TABLE.Intege = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo144) { //ExecuteSQLはWHERE句で指定したテーブル名の指定の一文字目の違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE UABLE1.Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo145) { //ExecuteSQLはWHERE句で指定したテーブル名の指定の二文字目の違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TBBLE1.Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo146) { //ExecuteSQLはWHERE句で指定したテーブル名の指定の最終文字の違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TABLE2.Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo147) { //ExecuteSQLはWHERE句で指定したテーブル名の指定が一文字多いという違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TABLE1a.Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo148) { //ExecuteSQLはWHERE句で指定したテーブル名の指定の一文字少ないという違いを見分けます。)
   const string sql =
        "SELECT * "
        "WHERE TABLE.Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_BAD_COLUMN_NAME, result);
}
TEST_F(MyTest, TestNo149) { //ExecuteSQLはWHERE句の後にORDER句を記述することができます。)
   const string sql =
        "SELECT * "
        "WHERE Integer <> 2 "
        "ORDER BY Integer DESC "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo150) { //ExecuteSQLはORDER句の後にWHERE句を記述することができます。)
   const string sql =
        "SELECT * "
        "ORDER BY Integer DESC "
        "WHERE Integer <> 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo151) { //ExecuteSQLはFROM句の後にSQLが続いたらERR_SQL_SYNTAXエラーとなります。)
   const string sql =
        "SELECT * "
        "FROM TABLE1 *";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo152) { //ExecuteSQLはWHERE句を二度記述するとERR_SQL_SYNTAXエラーとなります。)
   const string sql =
        "SELECT * "
        "WHERE Integer <> 2 "
        "WHERE Integer <> 2 "
        "FROM TABLE1";


    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo153) { //ExecuteSQLはORDER句を二度記述するとERR_SQL_SYNTAXエラーとなります。)
   const string sql =
        "SELECT * "
        "ORDER BY Integer DESC "
        "ORDER BY Integer DESC "
        "FROM TABLE1";


    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo154) { //ExecuteSQLはSELECTキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "select * "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo155) { //ExecuteSQLはFROMキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "from TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo156) { //ExecuteSQLはORDERキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "order BY String "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "2,B"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo157) { //ExecuteSQLはBYキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "ORDER by String "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "2,B"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo158) { //ExecuteSQLはASCキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1 asc, String2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "A,A"				"\n"
        "A,B"				"\n"
        "B,A"				"\n"
        "B,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo159) { //ExecuteSQLはDESCキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT String1, String2 "
        "ORDER BY String1 desc, String2 "
        "FROM UNORDERED2";

    string expectedCsv =
        "String1,String2"	"\n"
        "B,A"				"\n"
        "B,B"				"\n"
        "A,A"				"\n"
        "A,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo160) { //ExecuteSQLはWHEREキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "where Integer = 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo161) { //ExecuteSQLはANDキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "WHERE 1 < Integer and Integer < 3 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo162) { //ExecuteSQLはORキーワードを、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "WHERE Integer < 2 or 2 < Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo163) { //ExecuteSQLはFrom句のテーブル名を、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT * "
        "FROM table1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo164) { //ExecuteSQLはSELECT句のテーブルと一緒に指定した列名を、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT sTRING "
        "FROM table1";

    string expectedCsv =
        "String"	"\n"
        "A"			"\n"
        "B"			"\n"
        "C"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo165) { //ExecuteSQLはSELECT句のテーブル名を、大文字でも小文字でも識別します。)
   const string sql =
        "SELECT table1.String "
        "FROM TABLE1";

    string expectedCsv =
        "String"	"\n"
        "A"			"\n"
        "B"			"\n"
        "C"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo166) { //ExecuteSQLはORDER句のテーブルと一緒に指定した列名を、大文字でも小文字でも識別しじます。)
   const string sql =
        "SELECT * "
        "ORDER BY sTRING "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "2,B"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo167) { //ExecuteSQLはORDER句のテーブル名を、大文字でも小文字でも識別しじます。)
   const string sql =
        "SELECT * "
        "ORDER BY unordered.String "
        "FROM UNORDERED";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "11,AA"				"\n"
        "12,AB"				"\n"
        "2,B"				"\n"
        "21,BA"				"\n"
        "22,BB"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo168) { //ExecuteSQLは先頭がSELECTではなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "a SELECT * "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo169) { //ExecuteSQLはSELECTの次の語が識別子でもアスタリスクでもなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo170) { //ExecuteSQLはSELECT句のカンマの後が識別子でもアスタリスクでもなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT String, "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo171) { //ExecuteSQLはSELECT句のドットの後にテーブルと一緒に指定した列名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT TABLE1. "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo172) { //ExecuteSQLはSELECT句のドットの前にテーブル名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT .String "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo173) { //ExecuteSQLはORDERの後がBYでなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "ORDER b String"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo174) { //ExecuteSQLはBYの後が識別子でなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "ORDER BY BY"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo175) { //ExecuteSQLはORDER句のドットの後にテーブルと一緒に指定した列名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "ORDER BY TABLE1. "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo176) { //ExecuteSQLはORDER句のドットの前にテーブル名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "ORDER BY .String "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo177) { //ExecuteSQLはORDER句のカンマの後がの識別子でなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "ORDER BY String, "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo178) { //ExecuteSQLはWHEREの後が識別子でもリテラルでもなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "WHERE * = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo179) { //ExecuteSQLはWHERE句のドットの後にテーブルと一緒に指定した列名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "WHERE TABLE1. = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo180) { //ExecuteSQLはWHERE句のドットの前にテーブル名の記述がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "WHERE .Integer = 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo181) { //ExecuteSQLはWHERE句の左辺の後が演算子ではなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "WHERE Integer WHERE 2 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo182) { //ExecuteSQLはWHERE句の演算子の後が識別子でもリテラルでもなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "WHERE Integer = "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo183) { //ExecuteSQLはFROM句がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo184) { //ExecuteSQLはFROMの後に識別子がなかった場合にERR_SQL_SYNTAXを返します。)
   const string sql =
        "SELECT * "
        "FROM *";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo185) { //ExecuteSQLはSELECTの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT* "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo186) { //ExecuteSQLはドットの後にスペースがあっても問題なく動きます。)
   const string sql =
        "SELECT TABLE1. Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer"	"\n"
        "1"			"\n"
        "2"			"\n"
        "3"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo187) { //ExecuteSQLはドットの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT TABLE1.Integer "
        "FROM TABLE1";

    string expectedCsv =
        "Integer"	"\n"
        "1"			"\n"
        "2"			"\n"
        "3"			"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo188) { //ExecuteSQLはカンマの後にスペースがあっても問題なく動きます。)
   const string sql =
        "SELECT Integer, String "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo189) { //ExecuteSQLはカンマの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT Integer,String "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo190) { //ExecuteSQLはドットの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECTSTRING "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo191) { //ExecuteSQLはアスタリスクの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo192) { //ExecuteSQLはORDERの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT * "
        "ORDERBY Integer "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo193) { //ExecuteSQLはBYの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT * "
        "ORDER BYInteger "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo194) { //ExecuteSQLはASCの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT * "
        "ORDER BY Integer ASC"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo195) { //ExecuteSQLはDESCの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT * "
        "ORDER BY Integer DESC"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo196) { //ExecuteSQLはWHEREの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE\'B\' = String "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo197) { //ExecuteSQLはWHEREの後に文字が続くと整数リテラルとして読み込まれません。)
   const string sql =
        "SELECT *"
        "WHEREInteger = 2"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_TOKEN_CANT_READ, result);
}
TEST_F(MyTest, TestNo198) { //ExecuteSQLは識別子の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer= 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo199) { //ExecuteSQLは整数リテラルの後に文字が続くと整数リテラルとして読み込まれません。)
   const string sql =
        "SELECT *"
        "WHERE Integer = 2"
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_TOKEN_CANT_READ, result);
}
TEST_F(MyTest, TestNo200) { //ExecuteSQLは文字列リテラルの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE String = \'B\'"
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo201) { //ExecuteSQLは等しい記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer =2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo202) { //ExecuteSQLは等しくない記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT * "
        "WHERE Integer <>2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo203) { //ExecuteSQLは大なり記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT * "
        "WHERE Integer >2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo204) { //ExecuteSQLは小なり記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT * "
        "WHERE Integer <2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo205) { //ExecuteSQLは以上記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT * "
        "WHERE Integer >=2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo206) { //ExecuteSQLは以下記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT * "
        "WHERE Integer <=2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo207) { //ExecuteSQLは加算記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer +1 = 3 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo208) { //ExecuteSQLは減算記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer = 3 -1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo209) { //ExecuteSQLは乗算記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer = 2 *1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo210) { //ExecuteSQLは除算記号の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer = 2 /1 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo211) { //ExecuteSQLはAND演算子の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer < 3 AND\'A\' < String "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo212) { //ExecuteSQLはAND演算子の後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT *"
        "WHERE Integer < 3 ANDInteger > 1 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo213) { //ExecuteSQLはOR演算子の後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer >= 3  OR\'A\' >= String "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "1,A"				"\n"
        "3,C"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo214) { //ExecuteSQLはOR演算子の後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT *"
        "WHERE Integer >= 3 ORInteger <= 1 "
        "FROM TABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
TEST_F(MyTest, TestNo215) { //ExecuteSQLはカッコ開くの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE (Integer = 2) "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo216) { //ExecuteSQLはカッコ開くの後にスペースがあっても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE ( Integer = 2) "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo217) { //ExecuteSQLはカッコ閉じるの後にスペースがなくても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer = (2 - 1)* 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo218) { //ExecuteSQLはカッコ閉じるの後にスペースがあっても問題なく動きます。)
   const string sql =
        "SELECT *"
        "WHERE Integer = (2 - 1) * 2 "
        "FROM TABLE1";

    string expectedCsv =
        "Integer,String"	"\n"
        "2,B"				"\n";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)OK, result);
    EXPECT_EQ(expectedCsv, ReadOutput());
}
TEST_F(MyTest, TestNo219) { //ExecuteSQLはFROMの後にスペースを挟まずに文字が続くとキーワードとして読み込まれません。)
   const string sql =
        "SELECT *"
        "FROMTABLE1";

    auto result = ExecuteSQL(sql, testOutputPath);

    ASSERT_EQ((int)ERR_SQL_SYNTAX, result);
}
