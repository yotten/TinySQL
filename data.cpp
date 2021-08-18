#include "data.hpp"

#include <algorithm>
#include <cstring>

//! Dataクラスの新しいインスタンスを初期化
Data::Data()
{

}

//! Dataクラスの新しいインスタンスを初期化
Data::Data(const char* value)
{
    strncpy(this->value.string, value, std::max(MAX_DATA_LENGTH, MAX_WORD_LENGTH));
}

//! Dataクラスの新しいインスタンスを初期化
//! @param [in] value データの値
Data::Data(const int value) :type(DataType::INTEGER)
{
    this->value.integer = value;
}

//! Dataクラスの新しいインスタンスを初期化
//! @param [in] value データの値
Data::Data(const bool value) :type(DataType::BOOLEAN)
{
    this->value.boolean = value;
}

//! データが文字列型の場合の値を取得します。
//! @return データが文字列型の場合の値です。
const char* Data::string() const
{
    return value.string;
}

//! データが整数型の場合の値を取得します。
//! @return データが整数型の場合の値です。
const int Data::integer() const
{
    return value.integer;
}

//! データが真偽値型の場合の値を取得します。
//! @return データが真偽値型の場合の値です。
const int Data::boolean() const
{
    return value.boolean;
}