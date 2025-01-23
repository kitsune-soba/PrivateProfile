#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";
const std::string invalidPath = "hoge";
const std::string invalidLinePath = "../TestProfile/InvalidLine.ini";
const std::string emptyPath = "../TestProfile/Empty.ini";

std::string callbackResult;
size_t invalidLineCount;

void invalidLineCallback(const std::string& path, const size_t lineNumber, const std::string& line)
{
	++invalidLineCount;
	callbackResult = std::format("Invalid line : {} ({}) > {}", path, lineNumber, line);
}

}

TEST(LoadTest, PrimaryOnly)
{
	EXPECT_NO_THROW(const auto pp = pp::PrivateProfile(primaryPath));
	EXPECT_NO_THROW(const auto pp = pp::PrivateProfile(primaryPath, ""));
	EXPECT_THROW(const auto pp = pp::PrivateProfile(invalidPath), pp::ProfileLoadFailException);
}

TEST(LoadTest, PrimaryAndFallback)
{
	EXPECT_NO_THROW(pp::PrivateProfile(primaryPath, fallbackPath));
	EXPECT_THROW(pp::PrivateProfile(primaryPath, invalidPath), pp::ProfileLoadFailException);
	EXPECT_THROW(pp::PrivateProfile(invalidPath, fallbackPath), pp::ProfileLoadFailException);
}

TEST(LoadTest, InvalidLine)
{
	EXPECT_NO_THROW(pp::PrivateProfile(invalidLinePath, invalidLinePath));
}

TEST(LoadTest, InvalidLineCallback)
{
	{
		invalidLineCount = 0;
		callbackResult.clear();

		pp::PrivateProfile profile(invalidLinePath, invalidLineCallback);
		EXPECT_EQ(invalidLineCount, 3);
		EXPECT_EQ(callbackResult, "Invalid line : ../TestProfile/InvalidLine.ini (7) > Key1:Value1");
	}

	{
		invalidLineCount = 0;
		callbackResult.clear();

		pp::PrivateProfile profile(invalidLinePath, invalidLinePath, invalidLineCallback);
		EXPECT_EQ(invalidLineCount, 6);
		EXPECT_EQ(callbackResult, "Invalid line : ../TestProfile/InvalidLine.ini (7) > Key1:Value1");
	}
}

TEST(LoadTest, EmptyProfile)
{
	EXPECT_NO_THROW(pp::PrivateProfile(emptyPath, emptyPath));
}
