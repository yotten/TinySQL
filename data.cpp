#include "data.hpp"

#include <algorithm>
#include <string>

//! Dataクラスの新しいインスタンスを初期化
Data::Data() : m_value({0})
{

}

//! Dataクラスの新しいインスタンスを初期化
Data::Data(const std::string value) : m_value({0})
{
    m_string = value;
}

//! Dataクラスの新しいインスタンスを初期化
//! @param [in] value データの値
Data::Data(const int value) :type(DataType::INTEGER)
{
    m_value.integer = value;
}

//! Dataクラスの新しいインスタンスを初期化
//! @param [in] value データの値
Data::Data(const bool value) :type(DataType::BOOLEAN)
{
    m_value.boolean = value;
}

//! データが文字列型の場合の値を取得します。
//! @return データが文字列型の場合の値です。
const std::string& Data::string() const
{
    return m_string;
}

//! データが整数型の場合の値を取得します。
//! @return データが整数型の場合の値です。
const int Data::integer() const
{
    return m_value.integer;
}

//! データが真偽値型の場合の値を取得します。
//! @return データが真偽値型の場合の値です。
const int Data::boolean() const
{
    return m_value.boolean;
}