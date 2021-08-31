#pragma once

#include "column.hpp"
#include "token_kind.hpp"
#include "extension_tree_node.hpp"

#include <vector>
#include <string>
#include <memory>

class SqlQueryInfo
{
public:
	std::vector<std::string> tableNames; //!< FROM句で指定しているテーブル名です。
	std::vector<Column> selectColumns; //!< SELECT句に指定された列名です。
	std::vector<Column> orderByColumns; //!< ORDER句に指定された列名です。
	std::vector<TokenKind> orders; //!< 同じインデックスのorderByColumnsに対応している、昇順、降順の指定です。
	std::vector<std::shared_ptr<ExtensionTreeNode>> whereExtensionNodes; //!< WHEREに指定された木のノードを、木構造とは無関係に格納します。
	std::shared_ptr<ExtensionTreeNode> whereTopNode; //!< 式木の根となるノードです。
};