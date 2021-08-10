#include "column_index.hpp"

//! Columnクラスの新しいインスタンスを初期化します。
ColumnIndex::ColumnIndex() : ColumnIndex(0, 0)
{
}

//! Columnクラスの新しいインスタンスを初期化します。
//! @param [in] table 列が入力の何テーブル目の列かです。
//! @param [in] column 列が入力のテーブルの何列目かです。
ColumnIndex::ColumnIndex(const int table, const int column) : table(table), column(column)
{
}