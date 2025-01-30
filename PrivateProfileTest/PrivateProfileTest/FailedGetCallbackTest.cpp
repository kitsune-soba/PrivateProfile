#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string invalidLinePath = "../TestProfile/InvalidLine.ini";
const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";

std::string callbackResult;

void failedGetCallback(const std::string& path, const std::string& section, const std::string& key, const std::string& type, const pp::FailGetState state)
{
	switch (state)
	{
	case pp::FailGetState::NotFound:
		callbackResult = std::format("Not found : {} > [{}] > {}", path, section, key);
		break;
	case pp::FailGetState::TypeMismatch:
		callbackResult = std::format("Type mismatch ({}) : {} > [{}] > {}", type, path, section, key);
		break;
	case pp::FailGetState::UnexpectedOption:
		callbackResult = std::format("Unexpected option : {} > [{}] > {}", path, section, key);
		break;
	}
}

}

TEST(FailedGetCallbackTest, FailedGetPrimary)
{
	{
		callbackResult.clear();

		pp::PrivateProfile profile(primaryPath);
		profile.setFailedGetCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<int>("PrimaryOK_FallbackOK", "Integer"));
		EXPECT_TRUE(callbackResult.empty());
	}

	{
		callbackResult.clear();

		// �t�H�[���o�b�N�̒l���擾�ł����Ƃ��Ă��A�v���C�}������̎擾�ɂ͎��s���Ă���̂ŃR�[���o�b�N�����
		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<int>("PrimaryNG_FallbackOK", "Integer"));
		EXPECT_EQ(callbackResult, "Type mismatch (int) : ../TestProfile/Primary.ini > [PrimaryNG_FallbackOK] > Integer");
	}

	{
		callbackResult.clear();

		// �t�H�[���o�b�N�̒l���擾�ł����Ƃ��Ă��A�v���C�}������̎擾�ɂ͎��s���Ă���̂ŃR�[���o�b�N�����
		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<std::string>("PrimaryNG_FallbackOK", "Option", { "Gold", "Silver", "Bronze" }));
		EXPECT_EQ(callbackResult, "Unexpected option : ../TestProfile/Primary.ini > [PrimaryNG_FallbackOK] > Option");
	}
}

TEST(FailedGetCallbackTest, FailedGetFallback)
{
	{
		callbackResult.clear();

		// �v���C�}������l�����Ă���΁A�t�H�[���o�b�N�l�̎擾�����݂Ȃ��̂ŃR�[���o�b�N����Ȃ�
		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetCallback(failedGetCallback);
		EXPECT_TRUE(profile.get<int>("PrimaryOK_FallbackNG", "Integer"));
		EXPECT_TRUE(callbackResult.empty());
	}

	{
		callbackResult.clear();

		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetCallback(failedGetCallback);
		EXPECT_FALSE(profile.get<int>("PrimaryNG_FallbackNG", "Integer"));
		EXPECT_EQ(callbackResult, "Not found : ../TestProfile/Fallback.ini > [PrimaryNG_FallbackNG] > Integer");
	}

	{
		callbackResult.clear();

		pp::PrivateProfile profile(primaryPath, fallbackPath);
		profile.setFailedGetCallback(failedGetCallback);
		const auto result = profile.get<int, 3>("PrimaryNG_FallbackNG", "ArrayOrVector", ',');
		EXPECT_FALSE(result);
		EXPECT_EQ(callbackResult, std::format("Type mismatch ({}) : ../TestProfile/Fallback.ini > [PrimaryNG_FallbackNG] > ArrayOrVector", typeid(std::array<int, 3>).name()));
	}
}
