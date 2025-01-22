#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string path = "../TestProfile/FormatTest.ini";

}
TEST(FormatTest, Comment)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("Comment", "Key1");
	EXPECT_FALSE(value1);

	const auto value2 = profile.get<std::string>("Comment", "Key2");
	EXPECT_FALSE(value2);

	const auto value3 = profile.get<std::string>("Comment", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Velue3 ; This is not comment.");

	const auto value4 = profile.get<std::string>("Comment", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Velue4 # This is not comment.");
}

TEST(FormatTest, Section)
{
	const pp::PrivateProfile profile(path);

	// セクション名前後の空白はトリミングされる
	const auto value1 = profile.get<std::string>("Section", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// 同名のセクションがある場合、1つのキー・値のテーブルに合成される
	const auto value2 = profile.get<std::string>("Section", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	// 大文字・小文字は区別される
	const auto value3 = profile.get<std::string>("section", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("S e c t i o n", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Value4");

	// [ はセクション名として使用できない
	const auto value5 = profile.get<std::string>("Section[Fail", "Key5");
	EXPECT_FALSE(value5);

	// ] はセクション名として使用できない
	const auto value6 = profile.get<std::string>("Section]Fail", "Key6");
	EXPECT_FALSE(value6);
}

TEST(FormatTest, Key)
{
	const pp::PrivateProfile profile(path);

	// キーと値の区切りは : でもよい
	const auto value1 = profile.get<std::string>("Key", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// キー名前後の空白はトリミングされる
	const auto value2 = profile.get<std::string>("Key", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	// キー名前後の空白はトリミングされる
	const auto value3 = profile.get<std::string>("Key", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("Key", "K e y 4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Value4");

	// 大文字・小文字は区別される
	const auto VALUE5 = profile.get<std::string>("Key", "KEY5");
	EXPECT_TRUE(VALUE5);
	EXPECT_EQ(VALUE5.value(), "Value5-1");

	// 大文字・小文字は区別される
	const auto value5 = profile.get<std::string>("Key", "key5");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "Value5-2");

	// 同一セクションに複数の同名キーがある場合、いずれか1つの値のみを保持する
	// どの値を採用するかは選べない
	const auto value6 = profile.get<std::string>("Key", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_TRUE((value6.value() == "Value6-1") || (value6.value() == "Value6-2"));

	// キー名に [ ] は使用できない
	const auto value7 = profile.get<std::string>("Key", "Key[7]");
	EXPECT_FALSE(value7);
}

TEST(FormatTest, StringValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("StringValue", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// 値の前後の空白はトリミングされる
	const auto value2 = profile.get<std::string>("StringValue", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	const auto value3 = profile.get<std::string>("StringValue", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "V a l u e 3");

	// " で囲った場合でも " は値の一部とみなされる
	const auto value4 = profile.get<std::string>("StringValue", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "\"Value4\"");

	// ' で囲った場合でも ' は値の一部とみなされる
	const auto value5 = profile.get<std::string>("StringValue", "Key5");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "'Value5'");

	const auto value6 = profile.get<std::string>("StringValue", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_EQ(value6.value(), R"(=:;#[]		\\)");

	const auto value7 = profile.get<std::string>("StringValue", "Key7");
	EXPECT_TRUE(value7);
	EXPECT_EQ(value7.value(), R"(=:;#[]		\\)");
}

TEST(FormatTest, IntegerValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1_int = profile.get<int>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_int);
	EXPECT_EQ(value1_int.value(), 234);

	const auto value1_uint = profile.get<unsigned int>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_uint);
	EXPECT_EQ(value1_uint.value(), 234u);

	// char 型が表現できる範囲を超えているためパースに失敗する
	const auto value1_char = profile.get<char>("IntegerValue", "Key1");
	EXPECT_FALSE(value1_char);

	const auto value1_uchar = profile.get<unsigned char>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_uchar);
	EXPECT_EQ(value1_uchar.value(), 234);

	const auto value2_int = profile.get<int>("IntegerValue", "Key2");
	EXPECT_TRUE(value2_int);
	EXPECT_EQ(value2_int.value(), -234);

	// 負数は unsigned 型としてのパースに失敗する
	const auto value2_uint = profile.get<unsigned int>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uint);

	// char 型が表現できる範囲を超えているためパースに失敗する
	const auto value2_char = profile.get<char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_char);

	// 負数は unsigned 型としてのパースに失敗する
	const auto value2_uchar = profile.get<unsigned char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uchar);

	// 小数点数は整数型としてパースできない
	const auto value3 = profile.get<int>("IntegerValueFail", "Key3");
	EXPECT_FALSE(value3);

	const auto value4 = profile.get<int>("IntegerValueFail", "Key4");
	EXPECT_FALSE(value4);

	const auto value5 = profile.get<int>("IntegerValueFail", "Key5");
	EXPECT_FALSE(value5);

	const auto value6 = profile.get<int>("IntegerValueFail", "Key6");
	EXPECT_FALSE(value6);

	// 8進数リテラル的な表記には対応していない（一応パースは通るものの、期待と異なる結果と思われるため失敗のテストとする）
	const auto value7 = profile.get<int>("IntegerValueFail", "Key7");
	EXPECT_NE(value7.value(), 0777);

	// 16進数リテラル的な表記には対応していない
	const auto value8 = profile.get<int>("IntegerValueFail", "Key8");
	EXPECT_FALSE(value8);
}

TEST(FormatTest, FloatValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1_float = profile.get<float>("FloatValue", "Key1");
	EXPECT_TRUE(value1_float);
	EXPECT_FLOAT_EQ(value1_float.value(), 2.34f);

	const auto value1_double = profile.get<double>("FloatValue", "Key1");
	EXPECT_TRUE(value1_double);
	EXPECT_DOUBLE_EQ(value1_double.value(), 2.34);

	const auto value2 = profile.get<float>("FloatValue", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_FLOAT_EQ(value2.value(), -2.34f);

	const auto value3 = profile.get<float>("FloatValue", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_FLOAT_EQ(value3.value(), 234.f);

	const auto value4 = profile.get<float>("FloatValue", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_FLOAT_EQ(value4.value(), 234000.f);

	const auto value5 = profile.get<float>("FloatValue", "Key5");
	EXPECT_TRUE(value5);
	EXPECT_FLOAT_EQ(value5.value(), 234000.f);

	const auto value6 = profile.get<float>("FloatValue", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_FLOAT_EQ(value6.value(), 0.234f);

	// float で表現できない精度でもパースには成功する（丸められる）
	const auto value7_float = profile.get<float>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_float);
	EXPECT_FLOAT_EQ(value7_float.value(), 1.234568f);

	const auto value7_double = profile.get<double>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_double);
	EXPECT_DOUBLE_EQ(value7_double.value(), 1.234567890123456789);

	// 2.34e+100 は float ではオーバーフローするためパースできない
	const auto value8_float = profile.get<float>("DoubleValue", "Key8");
	EXPECT_FALSE(value8_float);

	// double ならパースに成功する
	const auto value8_double = profile.get<double>("DoubleValue", "Key8");
	EXPECT_TRUE(value8_double);
	EXPECT_DOUBLE_EQ(value8_double.value(), 2.34e+100);

	// 2.34e+100 は float ではアンダーフローするためパースできない（0 に丸められるわけではない）
	const auto value9_float = profile.get<float>("DoubleValue", "Key9");
	EXPECT_FALSE(value9_float);

	// double ならパースに成功する
	const auto value9_double = profile.get<double>("DoubleValue", "Key9");
	EXPECT_TRUE(value9_double);
	EXPECT_DOUBLE_EQ(value9_double.value(), 2.34e-100);
}

TEST(FormatTest, BooleanValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<bool>("BooleanValue", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_TRUE(value1.value());

	const auto value2 = profile.get<bool>("BooleanValue", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_FALSE(value2.value());

	// 大文字表記には非対応
	const auto value3 = profile.get<bool>("BooleanValueFail", "Key3");
	EXPECT_FALSE(value3);

	// 大文字表記には非対応
	const auto value4 = profile.get<bool>("BooleanValueFail", "Key4");
	EXPECT_FALSE(value4);

	// 大文字表記には非対応
	const auto value5 = profile.get<bool>("BooleanValueFail", "Key5");
	EXPECT_FALSE(value5);

	// 大文字表記には非対応
	const auto value6 = profile.get<bool>("BooleanValueFail", "Key6");
	EXPECT_FALSE(value6);

	// 数値表記には非対応
	const auto value7 = profile.get<bool>("BooleanValueFail", "Key7");
	EXPECT_FALSE(value7);

	// 数値表記には非対応
	const auto value8 = profile.get<bool>("BooleanValueFail", "Key8");
	EXPECT_FALSE(value8);
}

TEST(FormatTest, ArrayValue)
{
	const pp::PrivateProfile profile(path);

	// ini ファイルに書かれている要素数よりも少なく取れる
	const auto value1_1 = profile.get<int, 1>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_1);
	EXPECT_EQ(value1_1.value().at(0), 2);

	// ini ファイルに書かれている要素数よりも少なく取れる
	const auto value1_2 = profile.get<int, 2>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_2);
	EXPECT_EQ(value1_2.value().at(0), 2);
	EXPECT_EQ(value1_2.value().at(1), 3);

	const auto value1_3 = profile.get<int, 3>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_3);
	EXPECT_EQ(value1_3.value().at(0), 2);
	EXPECT_EQ(value1_3.value().at(1), 3);
	EXPECT_EQ(value1_3.value().at(2), 4);

	// ini ファイルに書かれている要素数よりも多く取ろうとするとパースに失敗する
	const auto value1_4 = profile.get<int, 4>("ArrayValue", "Key1", ',');
	EXPECT_FALSE(value1_4);

	const auto value2 = profile.get<float, 3>("ArrayValue", "Key2", ' ');
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value().at(0), 2.3f);
	EXPECT_EQ(value2.value().at(1), 4.5f);
	EXPECT_EQ(value2.value().at(2), 6.7f);

	const auto value3 = profile.get<bool, 2>("ArrayValue", "Key3", '\t');
	EXPECT_TRUE(value3);
	EXPECT_TRUE(value3.value().at(0));
	EXPECT_FALSE(value3.value().at(1));

	const auto value4 = profile.get<std::string, 2>("ArrayValue", "Key4", ';');
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value().at(0), "https://example.com/foo");
	EXPECT_EQ(value4.value().at(1), "https://example.com/bar");

	// ini ファイルに単一の値として書かれていても要素数 1 の std::array としてパースできる
	const auto value5 = profile.get<int, 1>("ArrayValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().at(0), 234);

	// 型が混在しているとパースに失敗する
	const auto value6 = profile.get<int, 4>("ArrayValueFail", "Key6", ',');
	EXPECT_FALSE(value6);

	// 空の要素があるとパースに失敗する
	const auto value7 = profile.get<int, 4>("ArrayValueFail", "Key7", ',');
	EXPECT_FALSE(value7);

	// 空の要素があるとパースに失敗する
	const auto value8 = profile.get<int, 3>("ArrayValueFail", "Key8", ',');
	EXPECT_FALSE(value8);

	// 要素に空白が付いているとパースに失敗する
	// （デリミタに空白文字が指定される可能性を考えると、各要素の前後の空白のトリムは無駄に複雑になりそうなので実装しない方針）
	const auto value9_int = profile.get<int, 3>("ArrayValueFail", "Key9", ',');
	EXPECT_FALSE(value9_int);

	// 但し、文字列型としてならパースできる
	const auto value9_string = profile.get<std::string, 3>("ArrayValueFail", "Key9", ',');
	EXPECT_TRUE(value9_string);
	EXPECT_EQ(value9_string.value().size(), 3);
	EXPECT_EQ(value9_string.value().at(0), "2");
	EXPECT_EQ(value9_string.value().at(1), " 3");
	EXPECT_EQ(value9_string.value().at(2), " 4");
}

TEST(FormatTest, VectorValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<int>("VectorValue", "Key1", ',');
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value().size(), 3);
	EXPECT_EQ(value1.value().at(0), 2);
	EXPECT_EQ(value1.value().at(1), 3);
	EXPECT_EQ(value1.value().at(2), 4);

	const auto value2 = profile.get<float>("VectorValue", "Key2", ' ');
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value().size(), 3);
	EXPECT_EQ(value2.value().at(0), 2.3f);
	EXPECT_EQ(value2.value().at(1), 4.5f);
	EXPECT_EQ(value2.value().at(2), 6.7f);

	const auto value3 = profile.get<bool>("VectorValue", "Key3", '\t');
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value().size(), 2);
	EXPECT_TRUE(value3.value().at(0));
	EXPECT_FALSE(value3.value().at(1));

	const auto value4 = profile.get<std::string>("VectorValue", "Key4", ';');
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value().size(), 2);
	EXPECT_EQ(value4.value().at(0), "https://example.com/foo");
	EXPECT_EQ(value4.value().at(1), "https://example.com/bar");

	// ini ファイルに単一の値として書かれていても要素数 1 の std::array としてパースできる
	const auto value5 = profile.get<int>("VectorValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().size(), 1);
	EXPECT_EQ(value5.value().at(0), 234);

	// 型が混在しているとパースに失敗する
	const auto value6 = profile.get<int>("VectorValueFail", "Key6", ',');
	EXPECT_FALSE(value6);

	// 空の要素があるとパースに失敗する
	const auto value7 = profile.get<int>("VectorValueFail", "Key7", ',');
	EXPECT_FALSE(value7);

	// 空の要素があるとパースに失敗する
	const auto value8 = profile.get<int>("VectorValueFail", "Key8", ',');
	EXPECT_FALSE(value8);

	// 要素に空白が付いているとパースに失敗する
	// （デリミタに空白文字が指定される可能性を考えると、各要素の前後の空白のトリムは無駄に複雑になりそうなので実装しない方針）
	const auto value9_int = profile.get<int>("VectorValueFail", "Key9", ',');
	EXPECT_FALSE(value9_int);

	// 但し、文字列型としてならパースできる
	const auto value9_string = profile.get<std::string>("VectorValueFail", "Key9", ',');
	EXPECT_TRUE(value9_string);
	EXPECT_EQ(value9_string.value().at(0), "2");
	EXPECT_EQ(value9_string.value().at(1), " 3");
	EXPECT_EQ(value9_string.value().at(2), " 4");
}
