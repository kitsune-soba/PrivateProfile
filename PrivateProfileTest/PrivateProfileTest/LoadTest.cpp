#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";
const std::string invalidPath = "hoge";
const std::string invalidFormatPath = "../TestProfile/invalidFormat.ini";
const std::string emptyPath = "../TestProfile/Empty.ini";

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

TEST(LoadTest, InvalidFormatProfile)
{
	EXPECT_NO_THROW(pp::PrivateProfile(invalidFormatPath, invalidFormatPath));
}

TEST(LoadTest, EmptyProfile)
{
	EXPECT_NO_THROW(pp::PrivateProfile(emptyPath, emptyPath));
}
