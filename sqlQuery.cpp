#include "sqlQuery.hpp"
#include "extension_tree_node.hpp"
#include "column_index.hpp"
#include "sqlQueryInfo.hpp"
#include "resultValue.hpp"
#include "intLiteralReader.hpp"
#include "stringLiteralReader.hpp"
#include "keywordReader.hpp"
#include "signReader.hpp"
#include "identifierReader.hpp"

using namespace std;

//! SqlQueryクラスの新しいインスタンスを初期化します。
//! @param [in] sql 実行するSQLです。
SqlQuery::SqlQuery(const string sql) :
// 先頭から順に検索されるので、前方一致となる二つの項目は順番に気をつけて登録しなくてはいけません。
	tokenReaders({
		make_shared<IntLiteralReader>(),
		make_shared<StringLiteralReader>(),
		make_shared<KeywordReader>(TokenKind::AND, "AND"),
		make_shared<KeywordReader>(TokenKind::ASC, "ASC"),
		make_shared<KeywordReader>(TokenKind::BY, "BY"),
		make_shared<KeywordReader>(TokenKind::DESC, "DESC"),
		make_shared<KeywordReader>(TokenKind::FROM, "FROM"),
		make_shared<KeywordReader>(TokenKind::ORDER, "ORDER"),
		make_shared<KeywordReader>(TokenKind::OR, "OR"),
		make_shared<KeywordReader>(TokenKind::SELECT, "SELECT"),
		make_shared<KeywordReader>(TokenKind::WHERE, "WHERE"),
		make_shared<SignReader>(TokenKind::GREATER_THAN_OR_EQUAL, ">="),
		make_shared<SignReader>(TokenKind::LESS_THAN_OR_EQUAL, "<="),
		make_shared<SignReader>(TokenKind::NOT_EQUAL, "<>"),
		make_shared<SignReader>(TokenKind::ASTERISK, "*"),
		make_shared<SignReader>(TokenKind::COMMA, ","),
		make_shared<SignReader>(TokenKind::CLOSE_PAREN, ")"),
		make_shared<SignReader>(TokenKind::DOT, "."),
		make_shared<SignReader>(TokenKind::EQUAL, "="),
		make_shared<SignReader>(TokenKind::GREATER_THAN, ">" ),
		make_shared<SignReader>(TokenKind::LESS_THAN, "<"),
		make_shared<SignReader>(TokenKind::MINUS, "-"),
		make_shared<SignReader>(TokenKind::OPEN_PAREN, "("),
		make_shared<SignReader>(TokenKind::PLUS, "+"),
		make_shared<SignReader>(TokenKind::SLASH, "/"),
		make_shared<IdentifierReader>(),
	}),
	operators({
		{ TokenKind::ASTERISK, 1 },
		{ TokenKind::SLASH, 1 },
		{ TokenKind::PLUS, 2 },
		{ TokenKind::MINUS, 2 },
		{ TokenKind::EQUAL, 3 },
		{ TokenKind::GREATER_THAN, 3 },
		{ TokenKind::GREATER_THAN_OR_EQUAL, 3 },
		{ TokenKind::LESS_THAN, 3 },
		{ TokenKind::LESS_THAN_OR_EQUAL, 3 },
		{ TokenKind::NOT_EQUAL, 3 },
		{ TokenKind::AND, 4 },
		{ TokenKind::OR, 5 }})
{
	auto tokens = GetTokens(sql);
	queryInfo = AnalyzeTokens(*tokens);
}

//! 二つの文字列を、大文字小文字を区別せずに比較し、等しいかどうかです。
//! @param [in] str1 比較される一つ目の文字列です。
//! @param [in] str2 比較される二つ目の文字列です。
//! @return 比較した結果、等しいかどうかです。
bool SqlQuery::Equali(const string str1, const string str2)
{
	bool ret;

	ret = str1.size() == str2.size() &&
		  equal(str1.begin(), str1.end(), str2.begin(),
		  [](const char &c1, const char &c2) {
			  return toupper(c1) == toupper(c2);
		  });

	return ret;
}

//! @param [in] sql トークンに分解する元となるSQLです。
//! @return 切り出されたトークンです。
const shared_ptr<vector<Token>> SqlQuery::GetTokens(const string sql) const
{
	auto cursol = sql.begin(); // SQLをトークンに分割して読み込む時に現在読んでいる文字の場所を表します。
	auto end = sql.end(); // sqlのendを指します。

	auto tokens = make_shared<vector<Token>>(); //読み込んだトークンです。

	// SQLをトークンに分割て読み込みます。
	while (cursol != end){

		// 空白を読み飛ばします。
		cursol = find_if(cursol, end, [&](char c){return space.find(c) == string::npos;});
		if (cursol == end) {
			break;
		}

		// 各種トークンを読み込み
		shared_ptr<const Token> token;
		if (any_of(tokenReaders.begin(), tokenReaders.end(),
			[&](const shared_ptr<const TokenReader>& reader) {
				return token = reader->Read(cursol, end);
			})) {
				tokens->push_back(*token);
		}
		else {
			throw ResultValue::ERR_TOKEN_CANT_READ;
		}
	}

	return tokens;
}

//! トークンを解析してSQLの構文で指定された情報を取得します。
//! @param [in] tokens 解析の対象となるトークンです。
//! @return 解析した結果の情報です。
const shared_ptr<const SqlQueryInfo> SqlQuery::AnalyzeTokens(const vector<Token> &tokens) const
{
	auto queryInfo = make_shared<SqlQueryInfo>();
	// トークン列を解析し、構文を読み取ります。
	auto tokenCursol = tokens.begin();
	bool readOrder = false; // すでにORDER句が読み込み済みかどうかです
	bool readWhere = false; // すでにWHERE句が読み込み済みかどうかです。
	bool first = true; // FROM句の最初のテーブル名を読み込み中かどうかです。

	// SQLの構文を解析し、必要な情報を取得します。

	// SELECT句を読み込みます。
	if (tokenCursol->kind == TokenKind::SELECT){
		++tokenCursol;
	}
	else{
		throw ResultValue::ERR_SQL_SYNTAX;
	}

	if (tokenCursol->kind == TokenKind::ASTERISK){
		++tokenCursol;
	}
	else
	{
		bool first = true; // SELECT句に最初に指定された列名の読み込みかどうかです。
		while (tokenCursol->kind == TokenKind::COMMA || first){
			if (tokenCursol->kind == TokenKind::COMMA){
				++tokenCursol;
			}
			if (tokenCursol->kind == TokenKind::IDENTIFIER){
				// テーブル名が指定されていない場合と仮定して読み込みます。
				queryInfo->selectColumns.push_back(Column(tokenCursol->word.c_str()));
				++tokenCursol;
				if (tokenCursol->kind == TokenKind::DOT){
					++tokenCursol;
					if (tokenCursol->kind == TokenKind::IDENTIFIER){
						// テーブル名が指定されていることがわかったので読み替えます。
						queryInfo->selectColumns.back() = Column(queryInfo->selectColumns.back().columnName, tokenCursol->word);
						++tokenCursol;
					}
					else{
						throw ResultValue::ERR_SQL_SYNTAX;
					}
				}
			}
			else{
				throw ResultValue::ERR_SQL_SYNTAX;
			}
			first = false;
		}
	}

	// ORDER句とWHERE句を読み込みます。最大各一回ずつ書くことができます。
	readOrder = false; // すでにORDER句が読み込み済みかどうかです。
	readWhere = false; // すでにWHERE句が読み込み済みかどうかです。
	while (tokenCursol->kind == TokenKind::ORDER || tokenCursol->kind == TokenKind::WHERE){

		// 二度目のORDER句はエラーです。
		if (readOrder && tokenCursol->kind == TokenKind::ORDER){
			throw ResultValue::ERR_SQL_SYNTAX;
		}

		// 二度目のWHERE句はエラーです。
		if (readWhere && tokenCursol->kind == TokenKind::WHERE){
			throw ResultValue::ERR_SQL_SYNTAX;
		}
		// ORDER句を読み込みます。
		if (tokenCursol->kind == TokenKind::ORDER){
			readOrder = true;
			++tokenCursol;
			if (tokenCursol->kind == TokenKind::BY){
				++tokenCursol;
				bool first = true; // ORDER句の最初の列名の読み込みかどうかです。
				while (tokenCursol->kind == TokenKind::COMMA || first){
					if (tokenCursol->kind == TokenKind::COMMA){
						++tokenCursol;
					}
					if (tokenCursol->kind == TokenKind::IDENTIFIER){
						// テーブル名が指定されていない場合と仮定して読み込みます。
						queryInfo->orderByColumns.push_back(Column(tokenCursol->word));
						++tokenCursol;
						if (tokenCursol->kind == TokenKind::DOT){
							++tokenCursol;
							if (tokenCursol->kind == TokenKind::IDENTIFIER) {
								// テーブル名が指定されていることがわかったので読み替えます。
								queryInfo->orderByColumns.back() = Column(queryInfo->orderByColumns.back().columnName, tokenCursol->word);
								++tokenCursol;
							}
							else{
								throw ResultValue::ERR_SQL_SYNTAX;
							}
						}

						// 並び替えの昇順、降順を指定します。
						if (tokenCursol->kind == TokenKind::ASC) {
							queryInfo->orders.push_back(TokenKind::ASC);
							++tokenCursol;
						}
						else if (tokenCursol->kind == TokenKind::DESC) {
							queryInfo->orders.push_back(TokenKind::DESC);
							++tokenCursol;
						}
						else {
							// 指定がない場合は昇順となります。
							queryInfo->orders.push_back(TokenKind::ASC);
						}
					}
					else{
						throw ResultValue::ERR_SQL_SYNTAX;
					}
					first = false;
				}
			}
			else{
				throw ResultValue::ERR_SQL_SYNTAX;
			}
		}

		// WHERE句を読み込みます。
		if (tokenCursol->kind == TokenKind::WHERE){
			readWhere = true;
			++tokenCursol;
			shared_ptr<ExtensionTreeNode> currentNode; // 現在読み込んでいるノードです。
			while (true){
				// オペランドを読み込みます。

				// オペランドのノードを新しく生成します。
				queryInfo->whereExtensionNodes.push_back(make_shared<ExtensionTreeNode>());
				if (currentNode){
					// 現在のノードを右の子にずらし、元の位置に新しいノードを挿入します。
					currentNode->right = queryInfo->whereExtensionNodes.back();
					currentNode->right->parent = currentNode;
					currentNode = currentNode->right;
				}
				else{
					// 最初はカレントノードに新しいノードを入れます。
					currentNode = queryInfo->whereExtensionNodes.back();
				}

				// カッコ開くを読み込みます。
				while (tokenCursol->kind == TokenKind::OPEN_PAREN){
					++currentNode->parenOpenBeforeClose;
					++tokenCursol;
				}

				// オペランドに前置される+か-を読み込みます。
				if (tokenCursol->kind == TokenKind::PLUS || tokenCursol->kind == TokenKind::MINUS){

					// +-を前置するのは列名と数値リテラルのみです。
					if (tokenCursol[1].kind != TokenKind::IDENTIFIER && tokenCursol[1].kind != TokenKind::INT_LITERAL){
						throw ResultValue::ERR_WHERE_OPERAND_TYPE;
					}
					if (tokenCursol->kind == TokenKind::MINUS){
						currentNode->signCoefficient = -1;
					}
					++tokenCursol;
				}

				// 列名、整数リテラル、文字列リテラルのいずれかをオペランドとして読み込みます。
				if (tokenCursol->kind == TokenKind::IDENTIFIER){

					// テーブル名が指定されていない場合と仮定して読み込みます。
					currentNode->column = Column(tokenCursol->word);
					++tokenCursol;
					if (tokenCursol->kind == TokenKind::DOT){
						++tokenCursol;
						if (tokenCursol->kind == TokenKind::IDENTIFIER){

							// テーブル名が指定されていることがわかったので読み替えます。
							currentNode->column = Column(currentNode->column.columnName, tokenCursol->word);
							++tokenCursol;
						}
						else{
							throw ResultValue::ERR_SQL_SYNTAX;
						}
					}
				}
				else if (tokenCursol->kind == TokenKind::INT_LITERAL){
					currentNode->value = Data(stoi(tokenCursol->word));
					++tokenCursol;
				}
				else if (tokenCursol->kind == TokenKind::STRING_LITERAL){
					// 前後のシングルクォートを取り去った文字列をデータとして読み込みます。
					currentNode->value = Data(string(tokenCursol->word).substr(1, string(tokenCursol->word).size() - 2));

					++tokenCursol;
				}
				else{
					throw ResultValue::ERR_SQL_SYNTAX;
				}

				// オペランドの右のカッコ閉じるを読み込みます。
				while (tokenCursol->kind == TokenKind::CLOSE_PAREN){
					shared_ptr<ExtensionTreeNode> searchedAncestor = currentNode->parent; // カッコ閉じると対応するカッコ開くを両方含む祖先ノードを探すためのカーソルです。
					while (searchedAncestor){

						// searchedAncestorの左の子に対応するカッコ開くがないかを検索します。
						shared_ptr<ExtensionTreeNode> searched = searchedAncestor; // searchedAncestorの内部からカッコ開くを検索するためのカーソルです。
						while (searched && !searched->parenOpenBeforeClose){
							searched = searched->left;
						}
						if (searched){
							// 対応付けられていないカッコ開くを一つ削除し、ノードがカッコに囲まれていることを記録します。
							--searched->parenOpenBeforeClose;
							searchedAncestor->inParen = true;
							break;
						}
						else{
							searchedAncestor = searchedAncestor->parent;
						}
					}
					++tokenCursol;
				}

				// 演算子(オペレーターを読み込みます。
				auto foundOperator = find_if(operators.begin(), operators.end(), [&](const Operator& op){ return op.kind == tokenCursol->kind; });

				// 現在見ている演算子の情報を探します。
				if (foundOperator != operators.end()) {
					// 見つかった演算子の情報をもとにノードを入れ替えます。
					shared_ptr<ExtensionTreeNode> tmp = currentNode; //ノードを入れ替えるために使う変数です。
					shared_ptr<ExtensionTreeNode> searched = tmp; // 入れ替えるノードを探すためのカーソルです。

					//カッコにくくられていなかった場合に、演算子の優先順位を参考に結合するノードを探します。
					bool first = true; // 演算子の優先順位を検索する最初のループです。
					do{
						if (!first){
							tmp = tmp->parent;
							searched = tmp;
						}
						// 現在の読み込み場所をくくるカッコが開く場所を探します。
						while (searched && !searched->parenOpenBeforeClose){
							searched = searched->left;
						}
						first = false;
					} while (!searched && tmp->parent && (tmp->parent->middleOperator.order <= foundOperator->order || tmp->parent->inParen));

					// 演算子のノードを新しく生成します。
					queryInfo->whereExtensionNodes.push_back(make_shared<ExtensionTreeNode>());
					currentNode = queryInfo->whereExtensionNodes.back();
					currentNode->middleOperator = *foundOperator;

					// 見つかった場所に新しいノードを配置します。これまでその位置にあったノードは左の子となるよう、親ノードと子ノードのポインタをつけかえます。
					currentNode->parent = tmp->parent;
					if (currentNode->parent){
						currentNode->parent->right = currentNode;
					}
					currentNode->left = tmp;
					tmp->parent = currentNode;

					++tokenCursol;
				}
				else{
					// 現在見ている種類が演算子の一覧から見つからなければ、WHERE句は終わります。
					break;
				}
			}

			// 木を根に向かってさかのぼり、根のノードを設定します。
			queryInfo->whereTopNode = currentNode;
			while (queryInfo->whereTopNode->parent){
				queryInfo->whereTopNode = queryInfo->whereTopNode->parent;
			}
		}
	}

	// FROM句を読み込みます。
	if (tokenCursol->kind == TokenKind::FROM){
		++tokenCursol;
	}
	else{
		throw ResultValue::ERR_SQL_SYNTAX;
	}

	first = true; // FROM句の最初のテーブル名を読み込み中かどうかです。
	while (tokenCursol != tokens.end() && tokenCursol->kind == TokenKind::COMMA || first){
		if (tokenCursol->kind == TokenKind::COMMA){
			++tokenCursol;
		}
		if (tokenCursol->kind == TokenKind::IDENTIFIER){
			queryInfo->tableNames.push_back(tokenCursol->word);
			++tokenCursol;
		}
		else{
			throw ResultValue::ERR_SQL_SYNTAX;
		}
		first = false;
	}

	// 最後のトークンまで読み込みが進んでいなかったらエラーです。
	if (tokenCursol != tokens.end()) {
		throw ResultValue::ERR_SQL_SYNTAX;
	}

	return queryInfo;
}

//! CSVファイルから入力データを読み取ります。
//! @return ファイルから読み取ったデータです。
const shared_ptr<const vector<InputTable>> SqlQuery::ReadCsv() const
{
	auto ret = make_shared<vector<InputTable>>();
	auto &tables = *ret;
	vector<ifstream> inputTableFiles; // 読み込む入力ファイルの全てのファイルポインタです。

	for (size_t i = 0; i < queryInfo->tableNames.size(); ++i){
		tables.push_back(InputTable());
		auto &table = tables.back();
		// 入力ファイルを開きます。
		inputTableFiles.push_back(ifstream(queryInfo->tableNames[i] + ".csv"));
		if (!inputTableFiles.back()) {
			throw ResultValue::ERR_FILE_OPEN;
		}

		// 入力CSVのヘッダ行を読み込みます。
		string inputLine; // ファイルから読み込んだ行文字列です。
		if (getline(inputTableFiles.back(), inputLine)) {
			auto charactorCursol = inputLine.begin();
			auto lineEnd = inputLine.end();

			// 読み込んだ行を最後まで読みます。
			while (charactorCursol != lineEnd){
				// 列名を一つ読みます。
				auto columnStart = charactorCursol;
				charactorCursol = find(charactorCursol, lineEnd, ',');
				table.columns.push_back(Column(queryInfo->tableNames[i], string(columnStart, charactorCursol)));
				// 入力行のカンマの分を読み進めます。
				if (charactorCursol != lineEnd) {
					++charactorCursol;
				}
			}
		}
		else{
			throw ResultValue::ERR_CSV_SYNTAX;
		}

		// 入力CSVのデータ行を読み込みます。
		while (getline(inputTableFiles.back(), inputLine)) {
			table.data.push_back(vector<Data>()); // 入力されている一行分のデータです。
			vector<Data> &row = table.data.back();

			auto charactorCursol = inputLine.begin(); // データ入力行を検索するカーソルです。
			auto lineEnd = inputLine.end(); // データ入力行のendを指します。
			
			// 読み込んだ行を最後まで読みます。
			while (charactorCursol != lineEnd){
				auto columnStart  = charactorCursol; // 現在の列の最初を記録しておきます。
				charactorCursol = find(charactorCursol, lineEnd, ',');
				
				row.push_back(Data(string(columnStart, charactorCursol)));

				// 入力行のカンマの分を読み進めます。
				if (charactorCursol != lineEnd) {
					++charactorCursol;
				}
			}
		}

		// 全てが数値となる列は数値列に変換します。
		for (size_t j = 0; j < table.columns.size(); ++j) {

			// 全ての行のある列について、データ文字列から符号と数値以外の文字を探します。

			// 符号と数字以外が見つからない列については、数値列に変換します。
			// none_of：無該当の時に真を返す。
			if (none_of(table.data.begin(), table.data.end(),
				[&](const vector<Data> &inputRow) {
					// any_of：条件式に部分一致すると真を返す。
					return any_of(inputRow[j].string().begin(), inputRow[j].string().end(),
						[&](const char& c) { return signNum.find(c) == string::npos; });
				})) {

				// 符号と数字以外が見つからない列については、数値列に変換します。
				for (auto& inputRow : table.data) {
					inputRow[j] = Data(stoi(inputRow[j].string()));
				}
			}
		}
	}
	for (auto &inputTableFile : inputTableFiles) {
		if (inputTableFile) {
			inputTableFile.close();
			if (inputTableFile.bad()) {
				throw ResultValue::ERR_FILE_CLOSE;
			}
		}
	}
	return ret;
}

//! CSVファイルに出力データを書き込みます。
void SqlQuery::WriteCsv(const string outputFileName, const vector<InputTable> &inputTables) const
{
	SqlQueryInfo info = *queryInfo;
	vector<Column> allInputColumns; // 入力に含まれるすべての列の一覧です。
	vector<vector<vector<Data>>::const_iterator> currentRows; // 入力された各テーブルの、現在出力している行を指すカーソルです。
	bool found;
	vector<vector<Data>> outputData; // 出力データです。
	vector<vector<Data>> allColumnOutputData; // 出力するデータに対応するインデックスを持ち、すべての入力データを保管します。
	ofstream outputFile; // 書き込むファイルのファイルポインタです。

	// 入力ファイルに書いてあったすべての列をallInputColumnsに設定します。
	for (size_t i = 0; i < info.tableNames.size(); ++i){
		transform(inputTables[i].columns.begin(), inputTables[i].columns.end(), back_inserter(allInputColumns),
			[&](const Column& column) { return Column(info.tableNames[i], column.columnName); });
	}

	// SELECT句の列名指定が*だった場合は、入力CSVの列名がすべて選択されます。
	if (info.selectColumns.empty()){
		copy(allInputColumns.begin(), allInputColumns.end(), back_inserter(info.selectColumns));
	}

	vector<Column> outputColumns;

	// SELECT句で指定された列名が、何個目の入力ファイルの何列目に相当するかを判別します。
	vector<ColumnIndex> selectColumnIndexes; // SELECT句で指定された列の、入力ファイルとしてのインデックスです。
	for (auto &selectColumn : info.selectColumns) {
		found = false;
		for (size_t i = 0; i < info.tableNames.size(); ++i){
			int j = 0;
			for (auto &inputColumn : inputTables[i].columns) {
				if (Equali(selectColumn.columnName, inputColumn.columnName) &&
					(selectColumn.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
					//!*selectTableNameCursol && !*inputTableNameCursol)){
					Equali(selectColumn.tableName, inputColumn.tableName))) {

					// 既に見つかっているのにもう一つ見つかったらエラーです。
					if (found){
						throw ResultValue::ERR_BAD_COLUMN_NAME;
					}
					found = true;
					// 見つかった値を持つ列のデータを生成します。
					selectColumnIndexes.push_back(ColumnIndex(i, j));
				}
				++j;
			}
		}

		// 一つも見つからなくてもエラーです。
		if (!found){
			throw ResultValue::ERR_BAD_COLUMN_NAME;
		}
	}

	// 出力する列名を設定します。
	transform(selectColumnIndexes.begin(), selectColumnIndexes.end(), back_inserter(outputColumns),
		[&](const ColumnIndex& index) {
			return inputTables[index.table].columns[index.column];
		});

	if (info.whereTopNode){
		// 既存数値の符号を計算します。
		for (auto &whereExtensionNode : info.whereExtensionNodes) {
			if (whereExtensionNode->middleOperator.kind == TokenKind::NOT_TOKEN &&
				whereExtensionNode->column.columnName.empty() &&
				whereExtensionNode->value.type == DataType::INTEGER) {
				whereExtensionNode->value = Data(whereExtensionNode->value.integer() * whereExtensionNode->signCoefficient);
			}
		}
	}

	transform(inputTables.begin(), inputTables.end(), back_inserter(currentRows),
		[](const InputTable& table) {
			return table.data.begin();
		});

	// 出力するデータを設定します。
	while (true){
		outputData.push_back(vector<Data>());
		vector<Data> &row = outputData.back(); // 出力している一行分のデータです。

		// 行の各列のデータを入力から持ってきて設定します。
		transform(selectColumnIndexes.begin(), selectColumnIndexes.end(), back_inserter(row),
			[&](const ColumnIndex& index) {
				return (*currentRows[index.table])[index.column];
			});

		allColumnOutputData.push_back(vector<Data>());
		vector<Data> &allColumnsRow = allColumnOutputData.back();// WHEREやORDERのためにすべての情報を含む行。rowとインデックスを共有します。
		for (auto &currentRow : currentRows) {
			copy(currentRow->begin(), currentRow->end(), back_inserter(allColumnsRow));
		}

		// WHEREの条件となる値を再帰的に計算します。
		if (info.whereTopNode){
			shared_ptr<ExtensionTreeNode> currentNode = info.whereTopNode; // 現在見ているノードです。
			while (currentNode){
				// 子ノードの計算が終わってない場合は、まずそちらの計算を行います。
				if (currentNode->left && !currentNode->left->calculated){
					currentNode = currentNode->left;
					continue;
				}
				else if (currentNode->right && !currentNode->right->calculated){
					currentNode = currentNode->right;
					continue;
				}

				// 自ノードの値を計算します。
				switch (currentNode->middleOperator.kind){
				case TokenKind::NOT_TOKEN:
					// ノードにデータが設定されている場合です。

					// データが列名で指定されている場合、今扱っている行のデータを設定します。
					if (!currentNode->column.columnName.empty()){
						found = false;
						for (size_t i = 0; i < allInputColumns.size(); ++i){
							if (Equali(currentNode->column.columnName, allInputColumns[i].columnName) &&
								(currentNode->column.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
								//!*whereTableNameCursol && !*allInputTableNameCursol)){
								Equali(currentNode->column.tableName, allInputColumns[i].tableName))) {
								// 既に見つかっているのにもう一つ見つかったらエラーです。
								if (found){
									throw ResultValue::ERR_BAD_COLUMN_NAME;
								}
								found = true;
								currentNode->value = allColumnsRow[i];
							}
						}
						// 一つも見つからなくてもエラーです。
						if (!found){
							throw ResultValue::ERR_BAD_COLUMN_NAME;
						}
						;
						// 符号を考慮して値を計算します。
						if (currentNode->value.type == DataType::INTEGER){
							currentNode->value = Data(currentNode->value.integer() * currentNode->signCoefficient);
						}
					}
					break;
				case TokenKind::EQUAL:
				case TokenKind::GREATER_THAN:
				case TokenKind::GREATER_THAN_OR_EQUAL:
				case TokenKind::LESS_THAN:
				case TokenKind::LESS_THAN_OR_EQUAL:
				case TokenKind::NOT_EQUAL:
					// 比較演算子の場合です。

					// 比較できるのは文字列型か整数型で、かつ左右の型が同じ場合です。
					if (currentNode->left->value.type != DataType::INTEGER && currentNode->left->value.type != DataType::STRING ||
						currentNode->left->value.type != currentNode->right->value.type){
						throw ResultValue::ERR_WHERE_OPERAND_TYPE;
					}
					currentNode->value.type = DataType::BOOLEAN;

					// 比較結果を型と演算子によって計算方法を変えて、計算します。
					switch (currentNode->left->value.type){
					case DataType::INTEGER:
						switch (currentNode->middleOperator.kind){
						case TokenKind::EQUAL:
							currentNode->value = Data(currentNode->left->value.integer() == currentNode->right->value.integer());
							break;
						case TokenKind::GREATER_THAN:
							currentNode->value = Data(currentNode->left->value.integer() > currentNode->right->value.integer());
							break;
						case TokenKind::GREATER_THAN_OR_EQUAL:
							currentNode->value = Data(currentNode->left->value.integer() >= currentNode->right->value.integer());
							break;
						case TokenKind::LESS_THAN:
							currentNode->value = Data(currentNode->left->value.integer() < currentNode->right->value.integer());
							break;
						case TokenKind::LESS_THAN_OR_EQUAL:
							currentNode->value = Data(currentNode->left->value.integer() <= currentNode->right->value.integer());
							break;
						case TokenKind::NOT_EQUAL:
							currentNode->value = Data(currentNode->left->value.integer() != currentNode->right->value.integer());
							break;
						}
						break;
					case DataType::STRING:
						switch (currentNode->middleOperator.kind){
						case TokenKind::EQUAL:
							currentNode->value = Data(currentNode->left->value.string().c_str() == currentNode->right->value.string());
							break;
						case TokenKind::GREATER_THAN:
							currentNode->value = Data(currentNode->left->value.string() > currentNode->right->value.string());
							break;
						case TokenKind::GREATER_THAN_OR_EQUAL:
							currentNode->value = Data(currentNode->left->value.string() >= currentNode->right->value.string());
							break;
						case TokenKind::LESS_THAN:
							currentNode->value = Data(currentNode->left->value.string() < currentNode->right->value.string());
							break;
						case TokenKind::LESS_THAN_OR_EQUAL:
							currentNode->value = Data(currentNode->left->value.string() <= currentNode->right->value.string());
							break;
						case TokenKind::NOT_EQUAL:
							currentNode->value = Data(currentNode->left->value.string() != currentNode->right->value.string());
							break;
						}
						break;
					}
					break;
				case TokenKind::PLUS:
				case TokenKind::MINUS:
				case TokenKind::ASTERISK:
				case TokenKind::SLASH:
					// 四則演算の場合です。

					// 演算できるのは整数型同士の場合のみです。
					if (currentNode->left->value.type != DataType::INTEGER || currentNode->right->value.type != DataType::INTEGER){
						throw ResultValue::ERR_WHERE_OPERAND_TYPE;
					}
					currentNode->value.type = DataType::INTEGER;

					// 比較結果を演算子によって計算方法を変えて、計算します。
					switch (currentNode->middleOperator.kind){
					case TokenKind::PLUS:
						currentNode->value = Data(currentNode->left->value.integer() + currentNode->right->value.integer());
						break;
					case TokenKind::MINUS:
						currentNode->value = Data(currentNode->left->value.integer() - currentNode->right->value.integer());
						break;
					case TokenKind::ASTERISK:
						currentNode->value = Data(currentNode->left->value.integer() * currentNode->right->value.integer());
						break;
					case TokenKind::SLASH:
						currentNode->value = Data(currentNode->left->value.integer() / currentNode->right->value.integer());
						break;
					}
					break;
				case TokenKind::AND:
				case TokenKind::OR:
					// 論理演算の場合です。

					// 演算できるのは真偽値型同士の場合のみです。
					if (currentNode->left->value.type != DataType::BOOLEAN || currentNode->right->value.type != DataType::BOOLEAN){
						throw ResultValue::ERR_WHERE_OPERAND_TYPE;
					}
					currentNode->value.type = DataType::BOOLEAN;

					// 比較結果を演算子によって計算方法を変えて、計算します。
					switch (currentNode->middleOperator.kind){
					case TokenKind::AND:
						currentNode->value = Data(currentNode->left->value.boolean() && currentNode->right->value.boolean());
						break;
					case TokenKind::OR:
						currentNode->value = Data(currentNode->left->value.boolean() || currentNode->right->value.boolean());
						break;
					}
				}
				currentNode->calculated = true;

				// 自身の計算が終わった後は親の計算に戻ります。
				currentNode = currentNode->parent;
			}

			// 条件に合わない行は出力から削除します。
			if (!info.whereTopNode->value.boolean()){
				allColumnOutputData.pop_back();
				outputData.pop_back();
			}
			// WHERE条件の計算結果をリセットします。
			for (auto &whereExtensionNode : info.whereExtensionNodes) {
				whereExtensionNode->calculated = false;
			}
		}

		// 各テーブルの行のすべての組み合わせを出力します。

		// 最後のテーブルのカレント行をインクリメントします。
		++currentRows[info.tableNames.size() - 1];

		// 最後のテーブルが最終行になっていた場合は先頭に戻し、順に前のテーブルのカレント行をインクリメントします。
		for (int i = info.tableNames.size() - 1; currentRows[i] == inputTables[i].data.end() && 0 < i; --i){
			++currentRows[i - 1];
			currentRows[i] = inputTables[i].data.begin();
		}

		// 最初のテーブルが最後の行を超えたなら出力行の生成は終わりです。
		if (currentRows[0] == inputTables[0].data.end()) {
			break;
		}
	}

	// ORDER句による並び替えの処理を行います。
	if (!info.orderByColumns.empty()){
		// ORDER句で指定されている列が、全ての入力行の中のどの行なのかを計算します。
		vector<int> orderByColumnIndexes; // ORDER句で指定された列の、すべての行の中でのインデックスです。

		for (auto &orderByColumn : info.orderByColumns) {
			found = false;
			for (size_t i = 0; i < allInputColumns.size(); ++i){
				if (Equali(orderByColumn.columnName, allInputColumns[i].columnName) &&
					(orderByColumn.tableName.empty() || // テーブル名が設定されている場合のみテーブル名の比較を行います。
					//!*orderByTableNameCursol && !*allInputTableNameCursol)){
					Equali(orderByColumn.tableName, allInputColumns[i].tableName))) {
					// 既に見つかっているのにもう一つ見つかったらエラーです。
					if (found){
						throw ResultValue::ERR_BAD_COLUMN_NAME;
					}
					found = true;
					orderByColumnIndexes.push_back(i);
				}
			}
			// 一つも見つからなくてもエラーです。
			if (!found){
				throw ResultValue::ERR_BAD_COLUMN_NAME;
			}
		}

		// outputDataとallColumnOutputDataのソートを一緒に行います。簡便のため凝ったソートは使わず、選択ソートを利用します。
		for (size_t i = 0; i < outputData.size(); ++i){
			int minIndex = i; // 現在までで最小の行のインデックスです。
			for (size_t j = i + 1; j < outputData.size(); ++j){
				bool jLessThanMin = false; // インデックスがjの値が、minIndexの値より小さいかどうかです。
				for (size_t k = 0; k < orderByColumnIndexes.size(); ++k){
					const Data &mData = allColumnOutputData[minIndex][orderByColumnIndexes[k]]; // インデックスがminIndexのデータです。
					const Data &jData = allColumnOutputData[j][orderByColumnIndexes[k]]; // インデックスがjのデータです。
					int cmp = 0; // 比較結果です。等しければ0、インデックスjの行が大きければプラス、インデックスminIndexの行が大きければマイナスとなります。
					switch (mData.type)
					{
					case DataType::INTEGER:
						cmp = jData.integer() - mData.integer();
						break;
					case DataType::STRING:
						cmp = strcmp(jData.string().c_str(), mData.string().c_str());
						break;
					}

					// 降順ならcmpの大小を入れ替えます。
					if (info.orders[k] == TokenKind::DESC){
						cmp *= -1;
					}
					if (cmp < 0){
						jLessThanMin = true;
						break;
					}
					else if (0 < cmp){
						break;
					}
				}
				if (jLessThanMin){
					minIndex = j;
				}
			}
			vector<Data> tmp = outputData[minIndex];
			outputData[minIndex] = outputData[i];
			outputData[i] = tmp;

			tmp = allColumnOutputData[minIndex];
			allColumnOutputData[minIndex] = allColumnOutputData[i];
			allColumnOutputData[i] = tmp;
		}
	}

	// 出力ファイルを開きます。
	outputFile = ofstream(outputFileName);
	if (outputFile.bad()){
		throw ResultValue::ERR_FILE_OPEN;
	}

	// 出力ファイルに列名を出力します。
	for (size_t i = 0; i < info.selectColumns.size(); ++i){
		outputFile << outputColumns[i].columnName;
		if (i < info.selectColumns.size() - 1){
			outputFile << ",";
		}
		else{
			outputFile << "\n";
		}
	}

	// 出力ファイルにデータを出力します。
	for (auto& outputRow : outputData) {
		size_t i = 0;
		for (auto &column : outputRow) {
			switch (column.type) {
			case DataType::INTEGER:
				outputFile << column.integer();
				break;
			case DataType::STRING:
				outputFile << column.string();
				break;
			}

			if (i++ < info.selectColumns.size() - 1){
				outputFile << ",";
			}
			else{
				outputFile << "\n";
			}
		}
	}
	if (outputFile.bad()){
		throw ResultValue::ERR_FILE_WRITE;
	}

	// 正常時の後処理です。

	// ファイルリソースを解放します。
	if (outputFile){
		outputFile.close();
		if (outputFile.bad()){
			throw ResultValue::ERR_FILE_CLOSE;
		}
	}
}

//! カレントディレクトリにあるCSVに対し、簡易的なSQLを実行し、結果をファイルに出力します。
//! @param[in] outputFileName SQLの実行結果をCSVとして出力するファイル名です。拡張子を含みます。
void SqlQuery::Execute(const string outputFileName)
{
	auto inputTables = ReadCsv();
	WriteCsv(outputFileName, *inputTables);
}
