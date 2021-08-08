#pragma once

//! 行の情報を入力のテーブルインデックス、列インデックスの形で持ちます。
class ColumnIndex {
public:
	int table;  //!< 列が入力の何テーブル目の列かです。
	int column; //!< 列が入力のテーブルの何列目かです。
};
