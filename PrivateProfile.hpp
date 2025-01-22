/*
	PrivateProfile.hpp

	ini�t�@�C���̒l��ǂݎ��N���X�B
	�o�[�W���� : 1.0.0
	�ڍׁ��ŐV�� : https://github.com/kitsune-soba/PrivateProfile

*/

#ifndef PP_PRIVATE_PROFILE_HPP__
#define PP_PRIVATE_PROFILE_HPP__

#include <array>
#include <charconv>
#include <format>
#include <fstream>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace pp
{

// ini �t�@�C���̓ǂݍ��ݎ��s���ɓ��������O
class ProfileLoadFailException : public std::runtime_error
{
public:
	ProfileLoadFailException(const std::string& message) : std::runtime_error(message) {}
};

class PrivateProfile
{
public:
	// �R���X�g���N�^
	PrivateProfile(const std::string& primaryProfilePath, const std::string& fallbackProfilePath = "")
		: primaryPath(primaryProfilePath), fallbackPath(fallbackProfilePath)
	{
		load();
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T>
	std::optional<T> get(const std::string& section, const std::string& key) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(T).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(T).name()); }

		return std::nullopt;
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T, Size>(primary, section, key, delimiter)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(std::array<T, Size>).name()); }

		if (const auto fallbackValue = getFrom<T, Size>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(std::array<T, Size>).name()); }

		return std::nullopt;
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T>
	std::optional<std::vector<T>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key, delimiter)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(std::vector<T>).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(std::vector<T>).name()); }

		return std::nullopt;
	}

	// �l�̎擾�Ɏ��s�����ۂ̃R�[���o�b�N��o�^
	using ValueGetFailCallback = std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)>; // path, sectijon, key, type
	void setPrimaryValueGetFailCallback(const ValueGetFailCallback&& callback)
	{ primaryValueGetFailCallback = callback; }
	void setFallbackValueGetFailCallback(const ValueGetFailCallback&& callback)
	{ fallbackValueGetFailCallback = callback; }

private:
	// ���K�\��
	const std::string sectionName = R"(\s*([^\[\]]*[^\[\]\s]+)\s*)";
	const std::string keyName = R"(\s*([^=:\[\]]*[^=:\[\]\s]+)\s*)";
	const std::string valueName = R"(\s*(.*[^\s]+)\s*)";
	const std::regex commentRegex{ "^[;#]" };
	const std::regex sectionRegex{ "^\\s*\\[" + sectionName + "\\]\\s*$" };
	const std::regex keyValueRegex{ "^" + keyName + "[=:]" + valueName + "$" };

	// �t�@�C���p�X
	const std::string primaryPath;
	const std::string fallbackPath;

	// �v���t�@�C���i�Z�N�V�����E�L�[�E�l�̃e�[�u���j
	using Profile = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>; // <section, <key, value>>
	Profile primary;
	Profile fallback;

	// �R�[���o�b�N
	ValueGetFailCallback primaryValueGetFailCallback;
	ValueGetFailCallback fallbackValueGetFailCallback;

	PrivateProfile() = delete;
	PrivateProfile(const PrivateProfile&) = delete;
	PrivateProfile& operator=(const PrivateProfile&) = delete;

	// �v���t�@�C�������[�h����
	// �p�X���w�肳��Ă���΁A�t�H�[���o�b�N�p�̃v���t�@�C�������[�h����
	void load(void)
	{
		loadFrom(primaryPath, primary);
		if (!fallbackPath.empty())
		{
			loadFrom(fallbackPath, fallback);
		}
	}

	// �w�肳�ꂽ�t�@�C���p�X�̃v���t�@�C�������[�h����
	void loadFrom(const std::string& path, Profile& profile)
	{
		std::ifstream ifs(path);
		if (!ifs)
		{
			throw ProfileLoadFailException(std::format("Failed to open {}", path));
		}

		std::smatch match;
		std::string line, currentSection;
		while (std::getline(ifs, line))
		{
			if (line.empty() || std::regex_match(line, match, commentRegex))
			{
				// no-op
			}
			else if (std::regex_match(line, match, keyValueRegex))
			{
				if (!currentSection.empty())
				{
					profile[currentSection][match[1]] = match[2];
				}
			}
			else if (std::regex_match(line, match, sectionRegex))
			{
				currentSection = match[1];
			}
		}
	}

	// �w�肳�ꂽ�v���t�@�C������l���擾����
	template <typename T>
	std::optional<T> getFrom(const Profile& profile, const std::string& section, const std::string& key) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T>(profile.at(section).at(key)) : std::nullopt;
	}

	// �w�肳�ꂽ�v���t�@�C������ std::array �𓾂�
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> getFrom(const Profile& profile, const std::string& section, const std::string& key, const char delimiter) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T, Size>(profile.at(section).at(key), delimiter) : std::nullopt;
	}

	// �w�肳�ꂽ�v���t�@�C������ std::vector �𓾂�
	template <typename T>
	std::optional<std::vector<T>> getFrom(const Profile& profile, const std::string& section, const std::string& key, const char delimiter) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T>(profile.at(section).at(key), delimiter) : std::nullopt;
	}

	// �����񂩂�l�֕ϊ�����
	template <typename T>
	std::optional<T> fromString(const std::string& valueString) const
	{
		T value;
		const char* const first = valueString.data();
		const char* const last = valueString.data() + valueString.size();
		const std::from_chars_result result = std::from_chars(first, last, value);
		return (result.ec == std::errc{} && result.ptr == last) ? std::optional<T>(value) : std::nullopt;
	}

	// �����񂩂� bool �l�֕ϊ�����
	template <>
	std::optional<bool> fromString(const std::string& valueString) const
	{
		if (valueString == "true") { return true; }
		else if (valueString == "false") { return false; }
		else { return std::nullopt; }
	}

	// �����񂩂�l�֕ϊ�����킯�ł͂Ȃ����A������^�����I�Ɉ�����悤�ɂ��邽�߂̓��ꉻ
	template <>
	std::optional<std::string> fromString(const std::string& valueString) const
	{
		return valueString;
	}

	// �����񂩂� std::array �֕ϊ�����
	template<typename T, unsigned int Size>
	std::optional<std::array<T, Size>> fromString(const std::string& valueString, char delimiter) const
	{
		std::istringstream iss(valueString);
		std::array<T, Size> result;
		std::string elementString;
		for (size_t i = 0; i < Size; ++i)
		{
			if (std::getline(iss, elementString, delimiter))
			{
				if (const auto element = fromString<T>(elementString)) { result.at(i) = *element; }
				else { return std::nullopt; }
			}
			else
			{
				return std::nullopt;
			}
		}
		return result;
	}

	// �����񂩂� std::vector �֕ϊ�����
	template<typename T>
	std::optional<std::vector<T>> fromString(const std::string& valueString, char delimiter) const
	{
		if (*valueString.rbegin() == delimiter) { return std::nullopt; } // ��������v�f�̏ꍇ�̓p�[�X���s�Ƃ���

		std::istringstream iss(valueString);
		std::vector<T> result;
		std::string elementString;
		while (std::getline(iss, elementString, delimiter))
		{
			if (const auto element = fromString<T>(elementString))
			{
				result.emplace_back(*element);
			}
			else
			{
				return std::nullopt;
			}
		}
		return result;
	}
};

}

#endif // PP_PRIVATE_PROFILE_HPP__
