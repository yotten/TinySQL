#include "data.hpp"
#include "token_kind.hpp"

#include <algorithm>
#include <cstring>

//! Dataクラスの新しいインスタンスを初期化
Data::Data()
{
    for (int i = 0; i < MAX_DATA_LENGTH; i++) {
        value.string[0] = 0x00;
    }
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