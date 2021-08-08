#pragma once

#include <token_kind.hpp>

//! 指定された列の情報です。どのテーブルに所属するかの情報も含みます。
class Column {
public:
	char tableName[MAX_WORD_LENGTH]; //!< 列が所属するテーブル名です。指定されていない場合は空文字列となります。
	char columnName[MAX_WORD_LENGTH]; //!< 指定された列の列名です。
};