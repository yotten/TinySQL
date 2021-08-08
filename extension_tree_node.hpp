#pragma once

#include "operator.hpp"
#include "column.hpp"
#include "data.hpp"

//! WHERE句の条件の式木を表します。
class ExtensionTreeNode {
public:
	struct ExtensionTreeNode *parent; //!< 親となるノードです。根の式木の場合はnullptrとなります。
	struct ExtensionTreeNode *left;   //!< 左の子となるノードです。自身が末端の葉となる式木の場合はnullptrとなります。
	Operator middleOperator;             //!< 中置される演算子です。自身が末端のとなる式木の場合の種類はNOT_TOKENとなります。
	struct ExtensionTreeNode *right;  //!< 右の子となるノードです。自身が末端の葉となる式木の場合はnullptrとなります。
	bool inParen;                        //!< 自身がかっこにくるまれているかどうかです。
	int parenOpenBeforeClose;            //!< 木の構築中に0以外となり、自身の左にあり、まだ閉じてないカッコの開始の数となります。
	int signCoefficient;                 //!< 自身が葉にあり、マイナス単項演算子がついている場合は-1、それ以外は1となります。
	Column column;                       //!< 列場指定されている場合に、その列を表します。列指定ではない場合はcolumnNameが空文字列となります。
	bool calculated;                     //!< 式の値を計算中に、計算済みかどうかです。
	Data value;                          //!< 指定された、もしくは計算された値です。
};