#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string path = "../TestProfile/FormatTest.ini";

}
TEST(GetTest, Comment)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("Comment", "Key1");
	EXPECT_FALSE(value1);

	const auto value2 = profile.get<std::string>("Comment", "Key2");
	EXPECT_FALSE(value2);

	const auto value3 = profile.get<std::string>("CommentFail", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Velue3 ; This is not comment.");

	const auto value4 = profile.get<std::string>("CommentFail", " #Key4");
	EXPECT_TRUE(value4);
}

TEST(GetTest, Section)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("Section", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	const auto value2 = profile.get<std::string>("Section", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	const auto value3 = profile.get<std::string>("section", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("\tS e c t i o n\t", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Value4");

	const auto value5 = profile.get<std::string>("[#;Section;#]", "Key5");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "Value5");

	const auto value6 = profile.get<std::string>(" ", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_EQ(value6.value(), "Value6");

	const auto value7 = profile.get<std::string>("Key=Value", "Key7");
	EXPECT_TRUE(value7);
	EXPECT_EQ(value7.value(), "Value7");

	const auto value8 = profile.get<std::string>("", "Key8");
	EXPECT_FALSE(value8);

	const auto value9 = profile.get<std::string>("SectionFail", "Key9");
	EXPECT_FALSE(value9);

	const auto value10 = profile.get<std::string>("SectionFail", "Key10");
	EXPECT_FALSE(value10);
}

TEST(GetTest, Key)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("Key", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_TRUE((value1.value() == "Value1A") || (value1.value() == "Value1B"));

	const auto VALUE1 = profile.get<std::string>("Key", "KEY1");
	EXPECT_TRUE(VALUE1);
	EXPECT_EQ(VALUE1.value(), "VALUE1");

	const auto value2 = profile.get<std::string>("Key", "\tKey2 ");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	const auto value3 = profile.get<std::string>("Key", "[#;Key3;#]");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("Key", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_TRUE(value4.value().empty());

	const auto value5 = profile.get<std::string>("Key", " ");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "Value5");

	const auto value6 = profile.get<std::string>("KeyFail", "");
	EXPECT_FALSE(value6);

	const auto value7 = profile.get<std::string>("KeyFail", "Key7");
	EXPECT_FALSE(value7);

	const auto value8 = profile.get<std::string>("KeyFail", "[Key8]");
	EXPECT_FALSE(value8);
}

TEST(GetTest, StringValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("StringValue", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	const auto value2 = profile.get<std::string>("StringValue", "Key2 ");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), " Value 2");

	const auto value3 = profile.get<std::string>("StringValue", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "\"Value3\"");

	const auto value4 = profile.get<std::string>("StringValue", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "'Value4'");

	const auto value5 = profile.get<std::string>("StringValue", "Key5");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "=;#[]\\\\\t");

	const auto value6 = profile.get<std::string>("StringValue", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_EQ(value6.value(), " ");

	const auto value7 = profile.get<std::string>("StringValue", "Key7");
	EXPECT_TRUE(value7);
	EXPECT_TRUE(value7.value().empty());
}

TEST(GetTest, IntegerValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1_int = profile.get<int>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_int);
	EXPECT_EQ(value1_int.value(), 234);

	const auto value1_uint = profile.get<unsigned int>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_uint);
	EXPECT_EQ(value1_uint.value(), 234u);

	const auto value1_char = profile.get<char>("IntegerValue", "Key1");
	EXPECT_FALSE(value1_char);

	const auto value1_uchar = profile.get<unsigned char>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_uchar);
	EXPECT_EQ(value1_uchar.value(), 234);

	const auto value2_int = profile.get<int>("IntegerValue", "Key2");
	EXPECT_TRUE(value2_int);
	EXPECT_EQ(value2_int.value(), -234);

	const auto value2_uint = profile.get<unsigned int>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uint);

	const auto value2_char = profile.get<char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_char);

	const auto value2_uchar = profile.get<unsigned char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uchar);

	const auto value3 = profile.get<int>("IntegerValue", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), 7777);

	const auto value4 = profile.get<int>("IntegerValueFail", "Key4");
	EXPECT_FALSE(value4);

	const auto value5 = profile.get<int>("IntegerValueFail", "Key5");
	EXPECT_FALSE(value5);

	const auto value6 = profile.get<int>("IntegerValueFail", "Key6");
	EXPECT_FALSE(value6);

	const auto value7 = profile.get<int>("IntegerValueFail", "Key7");
	EXPECT_FALSE(value7);

	const auto value8 = profile.get<int>("IntegerValueFail", "Key8");
	EXPECT_FALSE(value8);

	const auto value9 = profile.get<int>("IntegerValueFail", "Key9");
	EXPECT_FALSE(value9);

	const auto value10 = profile.get<int>("IntegerValueFail", "Key10");
	EXPECT_FALSE(value10);

	const auto value11 = profile.get<int>("IntegerValueFail", "Key11");
	EXPECT_FALSE(value11);
}

TEST(GetTest, FloatValue)
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

	const auto value7_float = profile.get<float>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_float);
	EXPECT_FLOAT_EQ(value7_float.value(), 1.234568f);

	const auto value7_double = profile.get<double>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_double);
	EXPECT_DOUBLE_EQ(value7_double.value(), 1.234567890123456789);

	const auto value8_float = profile.get<float>("DoubleValue", "Key8");
	EXPECT_FALSE(value8_float);

	const auto value8_double = profile.get<double>("DoubleValue", "Key8");
	EXPECT_TRUE(value8_double);
	EXPECT_DOUBLE_EQ(value8_double.value(), 2.34e+100);

	const auto value9_float = profile.get<float>("DoubleValue", "Key9");
	EXPECT_FALSE(value9_float);

	const auto value9_double = profile.get<double>("DoubleValue", "Key9");
	EXPECT_TRUE(value9_double);
	EXPECT_DOUBLE_EQ(value9_double.value(), 2.34e-100);

	const auto value10_float = profile.get<float>("FloatDoubleValueFail", "Key10");
	EXPECT_FALSE(value10_float);

	const auto value10_double = profile.get<double>("FloatDoubleValueFail", "Key10");
	EXPECT_FALSE(value10_double);

	const auto value11_float = profile.get<float>("FloatDoubleValueFail", "Key11");
	EXPECT_FALSE(value10_float);

	const auto value11_double = profile.get<double>("FloatDoubleValueFail", "Key11");
	EXPECT_FALSE(value11_double);
}

TEST(GetTest, BooleanValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<bool>("BooleanValue", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_TRUE(value1.value());

	const auto value2 = profile.get<bool>("BooleanValue", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_FALSE(value2.value());

	const auto value3 = profile.get<bool>("BooleanValueFail", "Key3");
	EXPECT_FALSE(value3);

	const auto value4 = profile.get<bool>("BooleanValueFail", "Key4");
	EXPECT_FALSE(value4);

	const auto value5 = profile.get<bool>("BooleanValueFail", "Key5");
	EXPECT_FALSE(value5);

	const auto value6 = profile.get<bool>("BooleanValueFail", "Key6");
	EXPECT_FALSE(value6);

	const auto value7 = profile.get<bool>("BooleanValueFail", "Key7");
	EXPECT_FALSE(value7);

	const auto value8 = profile.get<bool>("BooleanValueFail", "Key8");
	EXPECT_FALSE(value8);

	const auto value9 = profile.get<bool>("BooleanValueFail", "Key9");
	EXPECT_FALSE(value9);

	const auto value10 = profile.get<bool>("BooleanValueFail", "Key10");
	EXPECT_FALSE(value10);
}

TEST(GetTest, ArrayValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1_1 = profile.get<int, 1>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_1);
	EXPECT_EQ(value1_1.value().at(0), 2);

	const auto value1_2 = profile.get<int, 2>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_2);
	EXPECT_EQ(value1_2.value().at(0), 2);
	EXPECT_EQ(value1_2.value().at(1), 3);

	const auto value1_3 = profile.get<int, 3>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_3);
	EXPECT_EQ(value1_3.value().at(0), 2);
	EXPECT_EQ(value1_3.value().at(1), 3);
	EXPECT_EQ(value1_3.value().at(2), 4);

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

	const auto value5 = profile.get<int, 1>("ArrayValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().at(0), 234);

	const auto value6 = profile.get<std::string, 3>("ArrayValue", "Key6", ',');
	EXPECT_TRUE(value6);
	EXPECT_EQ(value6.value().at(0), "");
	EXPECT_EQ(value6.value().at(1), "");
	EXPECT_EQ(value6.value().at(2), "");

	const auto value7 = profile.get<int, 3>("ArrayValueFail", "Key7", ',');
	EXPECT_FALSE(value7);
	const auto value7_one = profile.get<int, 1>("ArrayValueFail", "Key7", ',');
	EXPECT_TRUE(value7_one);
	const auto value7_string = profile.get<std::string, 3>("ArrayValueFail", "Key7", ',');
	EXPECT_TRUE(value7_string);

	const auto value8 = profile.get<int, 4>("ArrayValueFail", "Key8", ',');
	EXPECT_FALSE(value8);
	const auto value8_three = profile.get<int, 3>("ArrayValueFail", "Key8", ',');
	EXPECT_TRUE(value8_three);
	const auto value8_string = profile.get<std::string, 4>("ArrayValueFail", "Key8", ',');
	EXPECT_TRUE(value8_string);

	const auto value9 = profile.get<int, 3>("ArrayValueFail", "Key9", ',');
	EXPECT_FALSE(value9);
	const auto value9_one = profile.get<int, 1>("ArrayValueFail", "Key9", ',');
	EXPECT_TRUE(value9_one);
	const auto value9_string = profile.get<std::string, 3>("ArrayValueFail", "Key9", ',');
	EXPECT_TRUE(value9_string);

	const auto value10_int = profile.get<int, 3>("ArrayValueFail", "Key10", ',');
	EXPECT_FALSE(value10_int);
	const auto value10_string = profile.get<std::string, 3>("ArrayValueFail", "Key10", ',');
	EXPECT_TRUE(value10_string);
}

TEST(GetTest, VectorValue)
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

	const auto value5 = profile.get<int>("VectorValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().size(), 1);
	EXPECT_EQ(value5.value().at(0), 234);

	const auto value6 = profile.get<std::string>("VectorValue", "Key6", ',');
	EXPECT_TRUE(value6);
	EXPECT_EQ(value6.value().size(), 3);
	EXPECT_EQ(value6.value().at(0), "");
	EXPECT_EQ(value6.value().at(1), "");
	EXPECT_EQ(value6.value().at(2), "");

	const auto value8 = profile.get<int>("VectorValueFail", "Key8", ',');
	EXPECT_FALSE(value8);
	const auto value8_string = profile.get<std::string>("VectorValueFail", "Key8", ',');
	EXPECT_TRUE(value8_string);
	EXPECT_EQ(value8_string.value().size(), 4);

	const auto value9 = profile.get<int>("VectorValueFail", "Key9", ',');
	EXPECT_FALSE(value9);
	const auto value9_string = profile.get<std::string>("VectorValueFail", "Key9", ',');
	EXPECT_TRUE(value9_string);
	EXPECT_EQ(value9_string.value().size(), 3);

	const auto value10_int = profile.get<int>("VectorValueFail", "Key10", ',');
	EXPECT_FALSE(value10_int);
	const auto value10_string = profile.get<std::string>("VectorValueFail", "Key10", ',');
	EXPECT_TRUE(value10_string);
	EXPECT_EQ(value9_string.value().size(), 3);
}

TEST(GetTest, Options)
{
	const pp::PrivateProfile profile(path);

	const auto stringValue = profile.get<std::string>("StringValue", "Key1", { "Value1", "Value2", "Value3" });
	EXPECT_TRUE(stringValue);
	EXPECT_EQ(stringValue.value(), "Value1");
	EXPECT_FALSE(profile.get<std::string>("StringValue", "Key1", { "ValueA", "ValueB", "ValueC" }));

	const auto integerValue = profile.get<int>("IntegerValue", "Key1", { 123, 234, 345 });
	EXPECT_TRUE(integerValue);
	EXPECT_EQ(integerValue.value(), 234);
	EXPECT_FALSE(profile.get<int>("IntegerValue", "Key1", { 0, 1, 2 }));

	const auto floatValue = profile.get<float>("FloatValue", "Key1", { 1.23f, 2.34f, 3.45f });
	EXPECT_TRUE(floatValue);
	EXPECT_EQ(floatValue.value(), 2.34f);
	EXPECT_FALSE(profile.get<float>("FloatValue", "Key1", { 0.f, 1.f, 2.f }));

	const auto booleanValue = profile.get<bool>("BooleanValue", "Key1", std::unordered_set<bool>{ true });
	EXPECT_TRUE(booleanValue);
	EXPECT_EQ(booleanValue.value(), true);
	EXPECT_FALSE(profile.get<bool>("BooleanValue", "Key1", std::unordered_set<bool>{ false }));
}
