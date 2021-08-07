#define MAX_DATA_LENGTH 256                //!< 入出力されるデータの、各列の最大長です。

//! 入力や出力、経過の計算に利用するデータのデータ型の種類を表します。
enum class DataType
{
	STRING,   //!< 文字列型です。
	INTEGER,  //!< 整数型です。
	BOOLEAN   //!< 真偽値型です。
};

//! 一つの値を持つデータです。
class Data {
public:
	DataType type; //!< データの型です。

	//! 実際のデータを格納する共用体です。
	union
	{
		char string[MAX_DATA_LENGTH]; //!< データが文字列型の場合の値です。
		int integer;                  //!< データが整数型の場合の値です。
		bool boolean;                 //!< データが真偽値型の場合の値です。
	} value;
};
