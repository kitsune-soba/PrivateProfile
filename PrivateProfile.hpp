/*
	PrivateProfile.hpp

	iniファイルの値を読み取るクラス。
	バージョン : 1.0.0
	詳細＆最新版 : https://github.com/kitsune-soba/PrivateProfile

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

// ini ファイルの読み込み失敗時に投げられる例外
class ProfileLoadFailException : public std::runtime_error
{
public:
	ProfileLoadFailException(const std::string& message) : std::runtime_error(message) {}
};

class PrivateProfile
{
public:
	// コンストラクタ
	PrivateProfile(const std::string& primaryProfilePath, const std::string& fallbackProfilePath = "")
		: primaryPath(primaryProfilePath), fallbackPath(fallbackProfilePath)
	{
		load();
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<T> get(const std::string& section, const std::string& key) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(T).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(T).name()); }

		return std::nullopt;
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T, Size>(primary, section, key, delimiter)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(std::array<T, Size>).name()); }

		if (const auto fallbackValue = getFrom<T, Size>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(std::array<T, Size>).name()); }

		return std::nullopt;
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<std::vector<T>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key, delimiter)) { return primaryValue; }
		else if (primaryValueGetFailCallback) { primaryValueGetFailCallback(primaryPath, section, key, typeid(std::vector<T>).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (fallbackValueGetFailCallback) { fallbackValueGetFailCallback(fallbackPath, section, key, typeid(std::vector<T>).name()); }

		return std::nullopt;
	}

	// 値の取得に失敗した際のコールバックを登録
	using ValueGetFailCallback = std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)>; // path, sectijon, key, type
	void setPrimaryValueGetFailCallback(const ValueGetFailCallback&& callback)
	{ primaryValueGetFailCallback = callback; }
	void setFallbackValueGetFailCallback(const ValueGetFailCallback&& callback)
	{ fallbackValueGetFailCallback = callback; }

private:
	// 正規表現
	const std::string sectionName = R"(\s*([^\[\]]*[^\[\]\s]+)\s*)";
	const std::string keyName = R"(\s*([^=:\[\]]*[^=:\[\]\s]+)\s*)";
	const std::string valueName = R"(\s*(.*[^\s]+)\s*)";
	const std::regex commentRegex{ "^[;#]" };
	const std::regex sectionRegex{ "^\\s*\\[" + sectionName + "\\]\\s*$" };
	const std::regex keyValueRegex{ "^" + keyName + "[=:]" + valueName + "$" };

	// ファイルパス
	const std::string primaryPath;
	const std::string fallbackPath;

	// プロファイル（セクション・キー・値のテーブル）
	using Profile = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>; // <section, <key, value>>
	Profile primary;
	Profile fallback;

	// コールバック
	ValueGetFailCallback primaryValueGetFailCallback;
	ValueGetFailCallback fallbackValueGetFailCallback;

	PrivateProfile() = delete;
	PrivateProfile(const PrivateProfile&) = delete;
	PrivateProfile& operator=(const PrivateProfile&) = delete;

	// プロファイルをロードする
	// パスが指定されていれば、フォールバック用のプロファイルもロードする
	void load(void)
	{
		loadFrom(primaryPath, primary);
		if (!fallbackPath.empty())
		{
			loadFrom(fallbackPath, fallback);
		}
	}

	// 指定されたファイルパスのプロファイルをロードする
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

	// 指定されたプロファイルから値を取得する
	template <typename T>
	std::optional<T> getFrom(const Profile& profile, const std::string& section, const std::string& key) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T>(profile.at(section).at(key)) : std::nullopt;
	}

	// 指定されたプロファイルから std::array を得る
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> getFrom(const Profile& profile, const std::string& section, const std::string& key, const char delimiter) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T, Size>(profile.at(section).at(key), delimiter) : std::nullopt;
	}

	// 指定されたプロファイルから std::vector を得る
	template <typename T>
	std::optional<std::vector<T>> getFrom(const Profile& profile, const std::string& section, const std::string& key, const char delimiter) const
	{
		return (profile.contains(section) && profile.at(section).contains(key)) ?
			fromString<T>(profile.at(section).at(key), delimiter) : std::nullopt;
	}

	// 文字列から値へ変換する
	template <typename T>
	std::optional<T> fromString(const std::string& valueString) const
	{
		T value;
		const char* const first = valueString.data();
		const char* const last = valueString.data() + valueString.size();
		const std::from_chars_result result = std::from_chars(first, last, value);
		return (result.ec == std::errc{} && result.ptr == last) ? std::optional<T>(value) : std::nullopt;
	}

	// 文字列から bool 値へ変換する
	template <>
	std::optional<bool> fromString(const std::string& valueString) const
	{
		if (valueString == "true") { return true; }
		else if (valueString == "false") { return false; }
		else { return std::nullopt; }
	}

	// 文字列から値へ変換するわけではないが、文字列型も画一的に扱えるようにするための特殊化
	template <>
	std::optional<std::string> fromString(const std::string& valueString) const
	{
		return valueString;
	}

	// 文字列から std::array へ変換する
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

	// 文字列から std::vector へ変換する
	template<typename T>
	std::optional<std::vector<T>> fromString(const std::string& valueString, char delimiter) const
	{
		if (*valueString.rbegin() == delimiter) { return std::nullopt; } // 末尾が空要素の場合はパース失敗とする

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
