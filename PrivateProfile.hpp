/*
	PrivateProfile.hpp

	ini�t�@�C���̒l��ǂݎ��N���X�B
	�o�[�W���� : 1.1.0
	�ڍׁ��ŐV�� : https://github.com/kitsune-soba/PrivateProfile

*/

#ifndef PP_PRIVATE_PROFILE_HPP__
#define PP_PRIVATE_PROFILE_HPP__

#include <array>
#include <charconv>
#include <fstream>
#include <functional>
#include <optional>
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
	using InvalidLineCallback = std::function<void(const std::string&, const size_t, const std::string&)>; // path, lineNumber, line
	using FailedGetCallback = std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)>; // path, sectijon, key, type

	// �R���X�g���N�^
	PrivateProfile(const std::string& primaryProfilePath, const InvalidLineCallback&& invalidLineCallback = InvalidLineCallback())
		: primaryPath(primaryProfilePath), fallbackPath(""), invalidLineCallback(invalidLineCallback)
	{
		load();
	}

	// �R���X�g���N�^
	PrivateProfile(const std::string& primaryProfilePath, const std::string& fallbackProfilePath, const InvalidLineCallback&& invalidLineCallback = InvalidLineCallback())
		: primaryPath(primaryProfilePath), fallbackPath(fallbackProfilePath), invalidLineCallback(invalidLineCallback)
	{
		load();
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T>
	std::optional<T> get(const std::string& section, const std::string& key) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(T).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(T).name()); }

		return std::nullopt;
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T, Size>(primary, section, key, delimiter)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(std::array<T, Size>).name()); }

		if (const auto fallbackValue = getFrom<T, Size>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(std::array<T, Size>).name()); }

		return std::nullopt;
	}

	// �v���t�@�C���̒l���擾
	// �v���C�}���v���t�@�C������l���擾�ł��Ȃ������ꍇ�A�t�H�[���o�b�N�v���t�@�C��������΂����炩��̎擾�����݂�
	template <typename T>
	std::optional<std::vector<T>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key, delimiter)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(std::vector<T>).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(std::vector<T>).name()); }

		return std::nullopt;
	}

	// �l�̎擾���s���̃R�[���o�b�N��ݒ�
	void setFailedGetPrimaryCallback(const FailedGetCallback&& callback)
	{ failedGetPrimaryCallback = callback; }
	void setFailedGetFallbackCallback(const FailedGetCallback&& callback)
	{ failedGetFallbackCallback = callback; }

private:
	// �t�@�C���p�X
	const std::string primaryPath;
	const std::string fallbackPath;

	// �v���t�@�C���i�Z�N�V�����E�L�[�E�l�̃e�[�u���j
	using Profile = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>; // <section, <key, value>>
	Profile primary;
	Profile fallback;

	// �R�[���o�b�N
	const InvalidLineCallback invalidLineCallback;
	FailedGetCallback failedGetPrimaryCallback;
	FailedGetCallback failedGetFallbackCallback;

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
			throw ProfileLoadFailException("Failed to open " + path);
		}

		std::string line, currentSection;
		size_t lineNumber = 0;
		while (std::getline(ifs, line))
		{
			++lineNumber;

			// ��s
			if (line.empty()) { continue; }

			const char& front = line.front();
			const char& back = line.back();
			const size_t equalPos = line.find_first_of('=');

			// �R�����g�s
			if ((front == ';') || (front == '#'))
			{
				continue;
			}
			// �Z�N�V�����s
			else if ((front == '[') && (back == ']') && (line.size() >= 3))
			{
				currentSection = line.substr(1, line.size() - 2);
			}
			// �L�[�E�l�̍s
			else if ((equalPos != std::string::npos) && (equalPos > 0))
			{
				const std::string key = line.substr(0, equalPos);
				const std::string value = (equalPos == line.size() - 1) ? "" : line.substr(equalPos + 1);
				profile[currentSection][key] = value;
			}
			// ���m�̃t�H�[�}�b�g
			else if (invalidLineCallback)
			{
				invalidLineCallback(path, lineNumber, line);
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
			else if (valueString.back() == delimiter)
			{
				// valueString �̖������f���~�^�ŏI���ꍇ�́A�Ō�ɋ󕶎���̗v�f������Ɖ��߂���
				if (const auto last = fromString<T>("")) { result.at(i) = *last; return result; }
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
		std::istringstream iss(valueString);
		std::vector<T> result;
		std::string elementString;
		while (std::getline(iss, elementString, delimiter))
		{
			if (const auto element = fromString<T>(elementString)) { result.emplace_back(*element); }
			else { return std::nullopt; }
		}

		// valueString �̖������f���~�^�ŏI���ꍇ�́A�Ō�ɋ󕶎���̗v�f������Ɖ��߂���
		if (valueString.back() == delimiter)
		{
			if (const auto last = fromString<T>("")) { result.emplace_back(*last); return result; }
			else { return std::nullopt; }
		}

		return result;
	}
};

}

#endif // PP_PRIVATE_PROFILE_HPP__
