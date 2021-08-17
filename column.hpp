#pragma once

#include <string>

//! 指定された列の情報です。どのテーブルに所属するかの情報も含みます。
class Column {
public:
	std::string tableName; //!< 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	std::string columnName; //!< 指定された列の列名です。
	
	//! Columnクラスの新しいインスタンスを初期化します。
	Column();

	//! Columnクラスの新しいインスタンスを初期化します。
	//! @param [in] columnName 指定された列の列名です。
	Column(const char* columnName);

	//! Columnクラスの新しいインスタンスを初期化します。
	//! @param [in] tableName 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	//! @param [in] columnName 指定された列の列名です。
	Column(const std::string tableName, const std::string columnName);
};