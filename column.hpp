#pragma once

#include "token_kind.hpp"
#include <string>

//! 指定された列の情報です。どのテーブルに所属するかの情報も含みます。
class Column {
public:
	//char tableName[MAX_WORD_LENGTH]; //!< 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	std::string tableName; //!< 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	char columnName[MAX_WORD_LENGTH]; //!< 指定された列の列名です。

	//! Columnクラスの新しいインスタンスを初期化します。
	Column();

	//! Columnクラスの新しいインスタンスを初期化します。
	//! @param [in] columnName 指定された列の列名です。
	Column(const char* columnName);

	//! Columnクラスの新しいインスタンスを初期化します。
	//! @param [in] tableName 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	//! @param [in] columnName 指定された列の列名です。
	Column(const std::string tableName, const char* columnName);
};