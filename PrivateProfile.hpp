/*
	PrivateProfile.hpp

	iniファイルの値を読み取るクラス。
	バージョン : 1.1.0
	詳細＆最新版 : https://github.com/kitsune-soba/PrivateProfile

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

// ini ファイルの読み込み失敗時に投げられる例外
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

	// コンストラクタ
	PrivateProfile(const std::string& primaryProfilePath, const InvalidLineCallback&& invalidLineCallback = InvalidLineCallback())
		: primaryPath(primaryProfilePath), fallbackPath(""), invalidLineCallback(invalidLineCallback)
	{
		load();
	}

	// コンストラクタ
	PrivateProfile(const std::string& primaryProfilePath, const std::string& fallbackProfilePath, const InvalidLineCallback&& invalidLineCallback = InvalidLineCallback())
		: primaryPath(primaryProfilePath), fallbackPath(fallbackProfilePath), invalidLineCallback(invalidLineCallback)
	{
		load();
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<T> get(const std::string& section, const std::string& key) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(T).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(T).name()); }

		return std::nullopt;
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T, Size>(primary, section, key, delimiter)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(std::array<T, Size>).name()); }

		if (const auto fallbackValue = getFrom<T, Size>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(std::array<T, Size>).name()); }

		return std::nullopt;
	}

	// プロファイルの値を取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<std::vector<T>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		if (const auto primaryValue = getFrom<T>(primary, section, key, delimiter)) { return primaryValue; }
		else if (failedGetPrimaryCallback) { failedGetPrimaryCallback(primaryPath, section, key, typeid(std::vector<T>).name()); }

		if (const auto fallbackValue = getFrom<T>(fallback, section, key, delimiter)) { return fallbackValue; }
		else if (failedGetFallbackCallback) { failedGetFallbackCallback(fallbackPath, section, key, typeid(std::vector<T>).name()); }

		return std::nullopt;
	}

	// 値の取得失敗時のコールバックを設定
	void setFailedGetPrimaryCallback(const FailedGetCallback&& callback)
	{ failedGetPrimaryCallback = callback; }
	void setFailedGetFallbackCallback(const FailedGetCallback&& callback)
	{ failedGetFallbackCallback = callback; }

private:
	// ファイルパス
	const std::string primaryPath;
	const std::string fallbackPath;

	// プロファイル（セクション・キー・値のテーブル）
	using Profile = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>; // <section, <key, value>>
	Profile primary;
	Profile fallback;

	// コールバック
	const InvalidLineCallback invalidLineCallback;
	FailedGetCallback failedGetPrimaryCallback;
	FailedGetCallback failedGetFallbackCallback;

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
			throw ProfileLoadFailException("Failed to open " + path);
		}

		std::string line, currentSection;
		size_t lineNumber = 0;
		while (std::getline(ifs, line))
		{
			++lineNumber;

			// 空行
			if (line.empty()) { continue; }

			const char& front = line.front();
			const char& back = line.back();
			const size_t equalPos = line.find_first_of('=');

			// コメント行
			if ((front == ';') || (front == '#'))
			{
				continue;
			}
			// セクション行
			else if ((front == '[') && (back == ']') && (line.size() >= 3))
			{
				currentSection = line.substr(1, line.size() - 2);
			}
			// キー・値の行
			else if ((equalPos != std::string::npos) && (equalPos > 0))
			{
				const std::string key = line.substr(0, equalPos);
				const std::string value = (equalPos == line.size() - 1) ? "" : line.substr(equalPos + 1);
				profile[currentSection][key] = value;
			}
			// 未知のフォーマット
			else if (invalidLineCallback)
			{
				invalidLineCallback(path, lineNumber, line);
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
			else if (valueString.back() == delimiter)
			{
				// valueString の末尾がデリミタで終わる場合は、最後に空文字列の要素があると解釈する
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

	// 文字列から std::vector へ変換する
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

		// valueString の末尾がデリミタで終わる場合は、最後に空文字列の要素があると解釈する
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
