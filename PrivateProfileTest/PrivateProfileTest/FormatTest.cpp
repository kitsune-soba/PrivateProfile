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

	// �Z�N�V�������O��̋󔒂̓g���~���O�����
	const auto value1 = profile.get<std::string>("Section", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// �����̃Z�N�V����������ꍇ�A1�̃L�[�E�l�̃e�[�u���ɍ��������
	const auto value2 = profile.get<std::string>("Section", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	// �啶���E�������͋�ʂ����
	const auto value3 = profile.get<std::string>("section", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("S e c t i o n", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Value4");

	// [ �̓Z�N�V�������Ƃ��Ďg�p�ł��Ȃ�
	const auto value5 = profile.get<std::string>("Section[Fail", "Key5");
	EXPECT_FALSE(value5);

	// ] �̓Z�N�V�������Ƃ��Ďg�p�ł��Ȃ�
	const auto value6 = profile.get<std::string>("Section]Fail", "Key6");
	EXPECT_FALSE(value6);
}

TEST(FormatTest, Key)
{
	const pp::PrivateProfile profile(path);

	// �L�[�ƒl�̋�؂�� : �ł��悢
	const auto value1 = profile.get<std::string>("Key", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// �L�[���O��̋󔒂̓g���~���O�����
	const auto value2 = profile.get<std::string>("Key", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	// �L�[���O��̋󔒂̓g���~���O�����
	const auto value3 = profile.get<std::string>("Key", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "Value3");

	const auto value4 = profile.get<std::string>("Key", "K e y 4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "Value4");

	// �啶���E�������͋�ʂ����
	const auto VALUE5 = profile.get<std::string>("Key", "KEY5");
	EXPECT_TRUE(VALUE5);
	EXPECT_EQ(VALUE5.value(), "Value5-1");

	// �啶���E�������͋�ʂ����
	const auto value5 = profile.get<std::string>("Key", "key5");
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value(), "Value5-2");

	// ����Z�N�V�����ɕ����̓����L�[������ꍇ�A�����ꂩ1�̒l�݂̂�ێ�����
	// �ǂ̒l���̗p���邩�͑I�ׂȂ�
	const auto value6 = profile.get<std::string>("Key", "Key6");
	EXPECT_TRUE(value6);
	EXPECT_TRUE((value6.value() == "Value6-1") || (value6.value() == "Value6-2"));

	// �L�[���� [ ] �͎g�p�ł��Ȃ�
	const auto value7 = profile.get<std::string>("Key", "Key[7]");
	EXPECT_FALSE(value7);
}

TEST(FormatTest, StringValue)
{
	const pp::PrivateProfile profile(path);

	const auto value1 = profile.get<std::string>("StringValue", "Key1");
	EXPECT_TRUE(value1);
	EXPECT_EQ(value1.value(), "Value1");

	// �l�̑O��̋󔒂̓g���~���O�����
	const auto value2 = profile.get<std::string>("StringValue", "Key2");
	EXPECT_TRUE(value2);
	EXPECT_EQ(value2.value(), "Value2");

	const auto value3 = profile.get<std::string>("StringValue", "Key3");
	EXPECT_TRUE(value3);
	EXPECT_EQ(value3.value(), "V a l u e 3");

	// " �ň͂����ꍇ�ł� " �͒l�̈ꕔ�Ƃ݂Ȃ����
	const auto value4 = profile.get<std::string>("StringValue", "Key4");
	EXPECT_TRUE(value4);
	EXPECT_EQ(value4.value(), "\"Value4\"");

	// ' �ň͂����ꍇ�ł� ' �͒l�̈ꕔ�Ƃ݂Ȃ����
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

	// char �^���\���ł���͈͂𒴂��Ă��邽�߃p�[�X�Ɏ��s����
	const auto value1_char = profile.get<char>("IntegerValue", "Key1");
	EXPECT_FALSE(value1_char);

	const auto value1_uchar = profile.get<unsigned char>("IntegerValue", "Key1");
	EXPECT_TRUE(value1_uchar);
	EXPECT_EQ(value1_uchar.value(), 234);

	const auto value2_int = profile.get<int>("IntegerValue", "Key2");
	EXPECT_TRUE(value2_int);
	EXPECT_EQ(value2_int.value(), -234);

	// ������ unsigned �^�Ƃ��Ẵp�[�X�Ɏ��s����
	const auto value2_uint = profile.get<unsigned int>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uint);

	// char �^���\���ł���͈͂𒴂��Ă��邽�߃p�[�X�Ɏ��s����
	const auto value2_char = profile.get<char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_char);

	// ������ unsigned �^�Ƃ��Ẵp�[�X�Ɏ��s����
	const auto value2_uchar = profile.get<unsigned char>("IntegerValue", "Key2");
	EXPECT_FALSE(value2_uchar);

	// �����_���͐����^�Ƃ��ăp�[�X�ł��Ȃ�
	const auto value3 = profile.get<int>("IntegerValueFail", "Key3");
	EXPECT_FALSE(value3);

	const auto value4 = profile.get<int>("IntegerValueFail", "Key4");
	EXPECT_FALSE(value4);

	const auto value5 = profile.get<int>("IntegerValueFail", "Key5");
	EXPECT_FALSE(value5);

	const auto value6 = profile.get<int>("IntegerValueFail", "Key6");
	EXPECT_FALSE(value6);

	// 8�i�����e�����I�ȕ\�L�ɂ͑Ή����Ă��Ȃ��i�ꉞ�p�[�X�͒ʂ���̂́A���҂ƈقȂ錋�ʂƎv���邽�ߎ��s�̃e�X�g�Ƃ���j
	const auto value7 = profile.get<int>("IntegerValueFail", "Key7");
	EXPECT_NE(value7.value(), 0777);

	// 16�i�����e�����I�ȕ\�L�ɂ͑Ή����Ă��Ȃ�
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

	// float �ŕ\���ł��Ȃ����x�ł��p�[�X�ɂ͐�������i�ۂ߂���j
	const auto value7_float = profile.get<float>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_float);
	EXPECT_FLOAT_EQ(value7_float.value(), 1.234568f);

	const auto value7_double = profile.get<double>("DoubleValue", "Key7");
	EXPECT_TRUE(value7_double);
	EXPECT_DOUBLE_EQ(value7_double.value(), 1.234567890123456789);

	// 2.34e+100 �� float �ł̓I�[�o�[�t���[���邽�߃p�[�X�ł��Ȃ�
	const auto value8_float = profile.get<float>("DoubleValue", "Key8");
	EXPECT_FALSE(value8_float);

	// double �Ȃ�p�[�X�ɐ�������
	const auto value8_double = profile.get<double>("DoubleValue", "Key8");
	EXPECT_TRUE(value8_double);
	EXPECT_DOUBLE_EQ(value8_double.value(), 2.34e+100);

	// 2.34e+100 �� float �ł̓A���_�[�t���[���邽�߃p�[�X�ł��Ȃ��i0 �Ɋۂ߂���킯�ł͂Ȃ��j
	const auto value9_float = profile.get<float>("DoubleValue", "Key9");
	EXPECT_FALSE(value9_float);

	// double �Ȃ�p�[�X�ɐ�������
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

	// �啶���\�L�ɂ͔�Ή�
	const auto value3 = profile.get<bool>("BooleanValueFail", "Key3");
	EXPECT_FALSE(value3);

	// �啶���\�L�ɂ͔�Ή�
	const auto value4 = profile.get<bool>("BooleanValueFail", "Key4");
	EXPECT_FALSE(value4);

	// �啶���\�L�ɂ͔�Ή�
	const auto value5 = profile.get<bool>("BooleanValueFail", "Key5");
	EXPECT_FALSE(value5);

	// �啶���\�L�ɂ͔�Ή�
	const auto value6 = profile.get<bool>("BooleanValueFail", "Key6");
	EXPECT_FALSE(value6);

	// ���l�\�L�ɂ͔�Ή�
	const auto value7 = profile.get<bool>("BooleanValueFail", "Key7");
	EXPECT_FALSE(value7);

	// ���l�\�L�ɂ͔�Ή�
	const auto value8 = profile.get<bool>("BooleanValueFail", "Key8");
	EXPECT_FALSE(value8);
}

TEST(FormatTest, ArrayValue)
{
	const pp::PrivateProfile profile(path);

	// ini �t�@�C���ɏ�����Ă���v�f���������Ȃ�����
	const auto value1_1 = profile.get<int, 1>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_1);
	EXPECT_EQ(value1_1.value().at(0), 2);

	// ini �t�@�C���ɏ�����Ă���v�f���������Ȃ�����
	const auto value1_2 = profile.get<int, 2>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_2);
	EXPECT_EQ(value1_2.value().at(0), 2);
	EXPECT_EQ(value1_2.value().at(1), 3);

	const auto value1_3 = profile.get<int, 3>("ArrayValue", "Key1", ',');
	EXPECT_TRUE(value1_3);
	EXPECT_EQ(value1_3.value().at(0), 2);
	EXPECT_EQ(value1_3.value().at(1), 3);
	EXPECT_EQ(value1_3.value().at(2), 4);

	// ini �t�@�C���ɏ�����Ă���v�f������������낤�Ƃ���ƃp�[�X�Ɏ��s����
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

	// ini �t�@�C���ɒP��̒l�Ƃ��ď�����Ă��Ă��v�f�� 1 �� std::array �Ƃ��ăp�[�X�ł���
	const auto value5 = profile.get<int, 1>("ArrayValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().at(0), 234);

	// �^�����݂��Ă���ƃp�[�X�Ɏ��s����
	const auto value6 = profile.get<int, 4>("ArrayValueFail", "Key6", ',');
	EXPECT_FALSE(value6);

	// ��̗v�f������ƃp�[�X�Ɏ��s����
	const auto value7 = profile.get<int, 4>("ArrayValueFail", "Key7", ',');
	EXPECT_FALSE(value7);

	// ��̗v�f������ƃp�[�X�Ɏ��s����
	const auto value8 = profile.get<int, 3>("ArrayValueFail", "Key8", ',');
	EXPECT_FALSE(value8);

	// �v�f�ɋ󔒂��t���Ă���ƃp�[�X�Ɏ��s����
	// �i�f���~�^�ɋ󔒕������w�肳���\�����l����ƁA�e�v�f�̑O��̋󔒂̃g�����͖��ʂɕ��G�ɂȂ肻���Ȃ̂Ŏ������Ȃ����j�j
	const auto value9_int = profile.get<int, 3>("ArrayValueFail", "Key9", ',');
	EXPECT_FALSE(value9_int);

	// �A���A������^�Ƃ��ĂȂ�p�[�X�ł���
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

	// ini �t�@�C���ɒP��̒l�Ƃ��ď�����Ă��Ă��v�f�� 1 �� std::array �Ƃ��ăp�[�X�ł���
	const auto value5 = profile.get<int>("VectorValue", "Key5", ';');
	EXPECT_TRUE(value5);
	EXPECT_EQ(value5.value().size(), 1);
	EXPECT_EQ(value5.value().at(0), 234);

	// �^�����݂��Ă���ƃp�[�X�Ɏ��s����
	const auto value6 = profile.get<int>("VectorValueFail", "Key6", ',');
	EXPECT_FALSE(value6);

	// ��̗v�f������ƃp�[�X�Ɏ��s����
	const auto value7 = profile.get<int>("VectorValueFail", "Key7", ',');
	EXPECT_FALSE(value7);

	// ��̗v�f������ƃp�[�X�Ɏ��s����
	const auto value8 = profile.get<int>("VectorValueFail", "Key8", ',');
	EXPECT_FALSE(value8);

	// �v�f�ɋ󔒂��t���Ă���ƃp�[�X�Ɏ��s����
	// �i�f���~�^�ɋ󔒕������w�肳���\�����l����ƁA�e�v�f�̑O��̋󔒂̃g�����͖��ʂɕ��G�ɂȂ肻���Ȃ̂Ŏ������Ȃ����j�j
	const auto value9_int = profile.get<int>("VectorValueFail", "Key9", ',');
	EXPECT_FALSE(value9_int);

	// �A���A������^�Ƃ��ĂȂ�p�[�X�ł���
	const auto value9_string = profile.get<std::string>("VectorValueFail", "Key9", ',');
	EXPECT_TRUE(value9_string);
	EXPECT_EQ(value9_string.value().at(0), "2");
	EXPECT_EQ(value9_string.value().at(1), " 3");
	EXPECT_EQ(value9_string.value().at(2), " 4");
}
