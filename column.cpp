#include "column.hpp"

#include <algorithm>
#include <cstring>
    
using namespace std;

//! Columnクラスの新しいインスタンスを初期化します。
Column::Column() : Column("", "")
{

}

//! Columnクラスの新しいインスタンスを初期化します。
//! @param [in] columnName 指定された列の列名です。
Column::Column(const char* columnName) : Column("", columnName)
{
}

//! Columnクラスの新しいインスタンスを初期化します。
//! @param [in] tableName 列が所属するテーブル名です。指定されていない場合は空文字列となります。
//! @param [in] columnName 指定された列の列名です。
Column::Column(const char* tableName, const char* columnName)
{
	strncpy(this->tableName, tableName, max(MAX_DATA_LENGTH, MAX_WORD_LENGTH));
	strncpy(this->columnName, columnName, max(MAX_DATA_LENGTH, MAX_WORD_LENGTH));
}