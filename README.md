# PrivateProfile.hpp

## 概要

ini ファイルの値を読み取るクラス。各種組み込み型や `std::array`、`std::vector` への変換機能も持つ。  
コンストラクタで ini ファイルのパスを2つ指定すると、1つ目の ini ファイルから値を取れなかった際に2つ目の ini ファイルの値へのフォールバックを試みる。  
値の取得に失敗した際のコールバックの設定も可能。  
ini ファイルへ値を書き込む機能は無い。

## 環境

- PrivateProfile.hpp : C++17 が利用できる環境であれば問題無いはず
- PrivateProfileTest : Visual Studio 2022

## 導入

[PrivateProfile.hpp](PrivateProfile.hpp) をインクルードするだけ。

## サンプルコード

```ini
; user_settings.ini
[Section]
Key1=Hello Private Profile
Key2=foobar
```

```ini
; default_settings.ini
[Section]
Key1=Default Value
Key2=1,1,1
```

```c++
const pp::PrivateProfile profile("user_settings.ini", "default_settings.ini");

if (const auto value = profile.get<std::string>("Section", "Key1"))
{
	// value の型は std::optional<std::string>
	std::cout << *value << std::endl; // 出力 : Hello Private Profile
}

if (const auto value = profile.get<int>("Section", "Key2", ',')) // ',' はデリミタ
{
	// value の型は std::optional<std::vector<int>>
	// user_settings.ini の値（foobar）は std::vector<int> としてのパースに失敗するため、
	// default_settings.ini の値へのフォールバックが試みられる
	std::cout << std::format("{}", *value) << std::endl; // 出力 : [1, 1, 1]
}
```

## 対応する ini ファイルのフォーマット

- 空行があってもよい
- コメントは行頭の `;` または `#` から行末まで（行頭以外からコメントを開始できない）
- 行の最も先頭側の `=` をキーと値の区切りとする
- セクション、キーは大文字・小文字を区別する
- 同名のセクションが複数ある場合、含まれるキーのリストが合成される
- セクション内に複数の同名キーがある場合、取得できる値はいずれか1つのみであり、どの値になるかは選べない
- セクション、キー、値の先頭や末尾の空白はトリミングされず、名前や値の一部と見做す
- セクション、キー、値でエスケープ処理は行われない（`\`, `"` 等の文字を使う場合はそのまま書けばよい）
- セクション、キー、値に ACSII 文字以外の文字を使用することは想定していない

対応するフォーマットの詳細は [FormatTest.ini](PrivateProfileTest/TestProfile/FormatTest.ini) を参照。
なお文字列から整数型や浮動小数点数型へのパースを `std::from_chars` に任せており、環境によって実行結果が微妙に変わる可能性がある。

