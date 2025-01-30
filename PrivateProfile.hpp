/*
	PrivateProfile.hpp

	iniファイルの値を読み取るクラス。
	バージョン : 1.2.0
	説明＆最新版 : https://github.com/kitsune-soba/PrivateProfile

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
#include <unordered_set>
#include <vector>

namespace pp
{

// ini ファイルの読み込み失敗時に投げられる例外
class ProfileLoadFailException : public std::runtime_error
{
public:
	ProfileLoadFailException(const std::string& message) : std::runtime_error(message) {}
};

enum class FailGetState
{
	NotFound, // セクション或いはキーが見つからない
	TypeMismatch, // 値を、指定された型に変換できない
	UnexpectedOption // 値が、指定されたオプションのいずれにも該当しない
};

// 特定のタイミングで呼び出されるコールバックの型
using InvalidLineCallback = std::function<void(const std::string&, const size_t, const std::string&)>; // path, lineNumber, line
using FailedGetCallback = std::function<void(const std::string&, const std::string&, const std::string&, const std::string&, FailGetState)>; // path, sectijon, key, type, FailGetState

class PrivateProfile
{
public:
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
		return getFrom<T>(primaryPath, section, key).
			or_else([&] { return getFrom<T>(fallbackPath, section, key); });
	}

	// プロファイルの値を取得（ini ファイルに書かれている値が options で指定した要素のいずれとも一致しない場合は取得しない。）
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<T> get(const std::string& section, const std::string& key, const std::unordered_set<T>& options) const
	{
		if (const auto value = getFrom<T>(primaryPath, section, key))
		{
			if (options.contains(*value))
			{
				return value;
			}
			else if (failedGetCallback)
			{
				failedGetCallback(primaryPath, section, key, typeid(T).name(), FailGetState::UnexpectedOption);
			}
		}

		if (const auto value = getFrom<T>(fallbackPath, section, key))
		{
			if (options.contains(*value))
			{
				return value;
			}
			else if (failedGetCallback)
			{
				failedGetCallback(fallbackPath, section, key, typeid(T).name(), FailGetState::UnexpectedOption);
			}
		}

		return std::nullopt;
	}

	// プロファイルの値を std::array として取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T, size_t Size>
	std::optional<std::array<T, Size>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		return getFrom<T, Size>(primaryPath, section, key, delimiter).
			or_else([&] { return getFrom<T, Size>(fallbackPath, section, key, delimiter); });
	}

	// プロファイルの値を std::vector として取得
	// プライマリプロファイルから値を取得できなかった場合、フォールバックプロファイルがあればそちらからの取得を試みる
	template <typename T>
	std::optional<std::vector<T>> get(const std::string& section, const std::string& key, char delimiter) const
	{
		return getFrom<T>(primaryPath, section, key, delimiter).
			or_else([&] { return getFrom<T>(fallbackPath, section, key, delimiter); });
	}

	// 値の取得失敗時のコールバックを設定
	void setFailedGetCallback(const FailedGetCallback&& callback)
	{
		failedGetCallback = callback;
	}

private:
	// ファイルパス
	const std::string primaryPath;
	const std::string fallbackPath;

	// プロファイル（セクション・キー・値のテーブル）
	using Profile = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>; // <section, <key, value>>
	std::unordered_map<std::string, Profile> profiles; // path, profile

	// コールバック
	const InvalidLineCallback invalidLineCallback;
	FailedGetCallback failedGetCallback;

	PrivateProfile() = delete;
	PrivateProfile(const PrivateProfile&) = delete;
	PrivateProfile& operator=(const PrivateProfile&) = delete;

	// プロファイルをロードする
	// パスが指定されていれば、フォールバック用のプロファイルもロードする
	void load(void)
	{
		loadFrom(primaryPath, profiles[primaryPath]);
		if (!fallbackPath.empty())
		{
			loadFrom(fallbackPath, profiles[fallbackPath]);
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
	std::optional<T> getFrom(const std::string& path, const std::string& section, const std::string& key) const
	{
		if (!profiles.contains(path)) { return std::nullopt; }

		const Profile& profile = profiles.at(path);
		if (!profile.contains(section) || !profile.at(section).contains(key))
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(T).name(), FailGetState::NotFound); }
			return std::nullopt;
		}
		else if (const auto value = fromString<T>(profile.at(section).at(key)))
		{
			return value;
		}
		else
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(T).name(), FailGetState::TypeMismatch); }
			return std::nullopt;
		}
	}

	// 指定されたプロファイルから std::array を得る
	template <typename T, unsigned int Size>
	std::optional<std::array<T, Size>> getFrom(const std::string& path, const std::string& section, const std::string& key, const char delimiter) const
	{
		if (!profiles.contains(path)) { return std::nullopt; }

		const Profile& profile = profiles.at(path);
		if (!profile.contains(section) || !profile.at(section).contains(key))
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(std::array<T, Size>).name(), FailGetState::NotFound); }
			return std::nullopt;
		}
		else if (const auto value = fromString<T, Size>(profile.at(section).at(key), delimiter))
		{
			return value;
		}
		else
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(std::array<T, Size>).name(), FailGetState::TypeMismatch); }
			return std::nullopt;
		}
	}

	// 指定されたプロファイルから std::vector を得る
	template <typename T>
	std::optional<std::vector<T>> getFrom(const std::string& path, const std::string& section, const std::string& key, const char delimiter) const
	{
		if (!profiles.contains(path)) { return std::nullopt; }

		const Profile& profile = profiles.at(path);
		if (!profile.contains(section) || !profile.at(section).contains(key))
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(std::vector<T>).name(), FailGetState::NotFound); }
			return std::nullopt;
		}
		else if (const auto value = fromString<T>(profile.at(section).at(key), delimiter))
		{
			return value;
		}
		else
		{
			if (failedGetCallback) { failedGetCallback(path, section, key, typeid(std::vector<T>).name(), FailGetState::TypeMismatch); }
			return std::nullopt;
		}
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
	template<typename T, size_t Size>
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
