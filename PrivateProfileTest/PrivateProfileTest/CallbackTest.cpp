#include "pch.h"
#include "../../PrivateProfile.hpp"

namespace
{

const std::string primaryPath = "../TestProfile/Primary.ini";
const std::string fallbackPath = "../TestProfile/Fallback.ini";

class CallbackTestException : public std::exception
{
public:
	CallbackTestException(const std::string& message) : message(message) {}
	const std::string& getMessage(void) const { return message; }

private:
	const std::string message;
};

void throwCallbackTestException(const std::string& path, const std::string& section, const std::string& key, const std::string& type)
{
	throw CallbackTestException(std::format("Failed to get {} value : {} > {} > {}", type, path, section, key));
}

}

TEST(CallbackTest, PrimaryFail)
{
	{
		pp::PrivateProfile profile("../TestProfile/Primary.ini");
		EXPECT_FALSE(profile.get<int>("PrimaryNG_FallbackOK", "Integer"));
		profile.setPrimaryValueGetFailCallback(throwCallbackTestException);
		EXPECT_THROW(profile.get<int>("PrimaryNG_FallbackOK", "Integer"), CallbackTestException);
	}

	{
		// �t�H�[���o�b�N�̒l���擾�ł����Ƃ��Ă��A�v���C�}������̎擾�ɂ͎��s���Ă���̂ŃR�[���o�b�N�����
		pp::PrivateProfile profile("../TestProfile/Primary.ini", "../TestProfile/Fallback.ini");
		EXPECT_TRUE(profile.get<int>("PrimaryNG_FallbackOK", "Integer"));
		profile.setPrimaryValueGetFailCallback(throwCallbackTestException);
		EXPECT_THROW(profile.get<int>("PrimaryNG_FallbackOK", "Integer"), CallbackTestException);
	}
}

TEST(CallbackTest, FallbackFail)
{
	{
		// �v���C�}������l�����Ă���΁A�t�H�[���o�b�N�l�̎擾�����݂Ȃ��̂ŃR�[���o�b�N����Ȃ�
		pp::PrivateProfile profile("../TestProfile/Primary.ini", "../TestProfile/Fallback.ini");
		EXPECT_TRUE(profile.get<int>("PrimaryOK_FallbackNG", "Integer"));
		profile.setFallbackValueGetFailCallback(throwCallbackTestException);
		EXPECT_NO_THROW(profile.get<int>("PrimaryOK_FallbackNG", "Integer"));
	}

	{
		pp::PrivateProfile profile("../TestProfile/Primary.ini", "../TestProfile/Fallback.ini");
		EXPECT_FALSE(profile.get<int>("PrimaryNG_FallbackNG", "Integer"));
		profile.setFallbackValueGetFailCallback(throwCallbackTestException);
		EXPECT_THROW(profile.get<int>("PrimaryNG_FallbackNG", "Integer"), CallbackTestException);
	}
}
