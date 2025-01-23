#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string invalidLinePath = "../TestProfile/InvalidLine.ini";
const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";

std::string callbackResult;

void failedGetCallback(const std::string& path, const std::string& section, const std::string& key, const std::string& type)
{
	callbackResult = std::format("Failed to get {} value : {} > [{}] > {}", type, path, section, key);
}

}

TEST(FailedGetCallbackTest, FailedGetPrimary)
{
	{
		callbackResult.clear();

		pp::PrivateProfile profile(primaryPath);
		profile.setFailedGetPrimaryCallback(failedGetCallback);
		EXPECT_FALSE(profile.get<int>("PrimaryNG_FallbackOK", "Integer"));
		EXPECT_EQ(callbackResult, "Failed to get int value : ../TestProfile/Primary.ini > [PrimaryNG_FallbackOK] > Integer");
	}

	{
		callbackResult.clear();

		// フォールバックの値を取得できたとしても、プライマリからの取得には失敗しているのでコールバックされる
		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetPrimaryCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<int>("PrimaryNG_FallbackOK", "Integer"));
		EXPECT_EQ(callbackResult, "Failed to get int value : ../TestProfile/Primary.ini > [PrimaryNG_FallbackOK] > Integer");
	}
}

TEST(FailedGetCallbackTest, FailedGetFallback)
{
	{
		callbackResult.clear();

		// プライマリから値を取れていれば、フォールバック値の取得を試みないのでコールバックされない
		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetFallbackCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<int>("PrimaryOK_FallbackNG", "Integer"));
		EXPECT_TRUE(callbackResult.empty());
	}

	{
		callbackResult.clear();

		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetFallbackCallback(failedGetCallback);
		EXPECT_FALSE(profile.get<int>("PrimaryNG_FallbackNG", "Integer"));
		EXPECT_EQ(callbackResult, "Failed to get int value : ../TestProfile/Fallback.ini > [PrimaryNG_FallbackNG] > Integer");
	}
}
