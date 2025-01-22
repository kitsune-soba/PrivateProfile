#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";

}

// プライマリから値を取れた場合、フォールバックのフォーマットが正しくてもプライマリの値が返る
TEST(FallbackTest, PrimaryOK_FallbackOK)
{
	const pp::PrivateProfile profile(primaryPath, fallbackPath);
	const std::string section = "PrimaryOK_FallbackOK";

	const auto integerValue = profile.get<int>(section, "Integer");
	EXPECT_TRUE(integerValue);
	EXPECT_EQ(integerValue.value(), 234);

	const auto boolednValue = profile.get<bool>(section, "Boolean");
	EXPECT_TRUE(boolednValue);
	EXPECT_EQ(boolednValue.value(), true);

	const auto arrayValue = profile.get<int, 3>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(arrayValue);
	EXPECT_EQ(arrayValue.value().at(0), 2);
	EXPECT_EQ(arrayValue.value().at(1), 3);
	EXPECT_EQ(arrayValue.value().at(2), 4);

	const auto vectorValue = profile.get<int>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(vectorValue);
	EXPECT_EQ(vectorValue.value().size(), 3);
	EXPECT_EQ(vectorValue.value().at(0), 2);
	EXPECT_EQ(vectorValue.value().at(1), 3);
	EXPECT_EQ(vectorValue.value().at(2), 4);
}

// プライマリから値を取れた場合、フォールバックのフォーマットが不正でもプライマリの値が返る
TEST(FallbackTest, PrimaryOK_FallbackNG)
{
	const pp::PrivateProfile profile(primaryPath, fallbackPath);
	const std::string section = "PrimaryOK_FallbackNG";

	const auto integerValue = profile.get<int>(section, "Integer");
	EXPECT_TRUE(integerValue);
	EXPECT_EQ(integerValue.value(), 234);

	const auto boolednValue = profile.get<bool>(section, "Boolean");
	EXPECT_TRUE(boolednValue);
	EXPECT_EQ(boolednValue.value(), true);

	const auto arrayValue = profile.get<int, 3>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(arrayValue);
	EXPECT_EQ(arrayValue.value().at(0), 2);
	EXPECT_EQ(arrayValue.value().at(1), 3);
	EXPECT_EQ(arrayValue.value().at(2), 4);

	const auto vectorValue = profile.get<int>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(vectorValue);
	EXPECT_EQ(vectorValue.value().size(), 3);
	EXPECT_EQ(vectorValue.value().at(0), 2);
	EXPECT_EQ(vectorValue.value().at(1), 3);
	EXPECT_EQ(vectorValue.value().at(2), 4);
}

// プライマリから値を取れない場合、フォールバックのから値を取れればこれを返す
TEST(FallbackTest, PrimaryNG_FallbackOK)
{
	const pp::PrivateProfile profile(primaryPath, fallbackPath);
	const std::string section = "PrimaryNG_FallbackOK";

	const auto integerValue = profile.get<int>(section, "Integer");
	EXPECT_TRUE(integerValue);
	EXPECT_EQ(integerValue.value(), 432);

	const auto boolednValue = profile.get<bool>(section, "Boolean");
	EXPECT_TRUE(boolednValue);
	EXPECT_EQ(boolednValue.value(), false);

	const auto arrayValue = profile.get<int, 3>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(arrayValue);
	EXPECT_EQ(arrayValue.value().at(0), 4);
	EXPECT_EQ(arrayValue.value().at(1), 3);
	EXPECT_EQ(arrayValue.value().at(2), 2);

	const auto vectorValue = profile.get<int>(section, "ArrayOrVector", ',');
	EXPECT_TRUE(vectorValue);
	EXPECT_EQ(vectorValue.value().size(), 3);
	EXPECT_EQ(vectorValue.value().at(0), 4);
	EXPECT_EQ(vectorValue.value().at(1), 3);
	EXPECT_EQ(vectorValue.value().at(2), 2);
}

// プライマリからもフォールバックのからも値を取れない場合は std::nullopt が返る
TEST(FallbackTest, PrimaryNG_FallbackNG)
{
	const pp::PrivateProfile profile(primaryPath, fallbackPath);
	const std::string section = "PrimaryNG_FallbackNG";

	const auto integerValue = profile.get<int>(section, "Integer");
	EXPECT_FALSE(integerValue);

	const auto boolednValue = profile.get<bool>(section, "Boolean");
	EXPECT_FALSE(boolednValue);

	const auto arrayValue = profile.get<int, 3>(section, "ArrayOrVector", ',');
	EXPECT_FALSE(arrayValue);

	const auto vectorValue = profile.get<int>(section, "ArrayOrVector", ',');
	EXPECT_FALSE(vectorValue);
}
