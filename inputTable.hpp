#pragma once

#include "column.hpp"
#include "data.hpp"
#include <vector>

//! CSVとして入力されたファイルの内容を表します。
class InputTable
{
public:
	std::vector<Column> columns; //!< 列の情報です。
	std::vector<std::vector< Data>> data; //! データです。
};