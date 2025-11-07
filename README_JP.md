# 𓆩⚚𓆪 Orbis
> [!CAUTION]
> プロジェクト Orbis はまだ開発初期段階です。現状、プロダクションレベルの開発に使用することはおすすめしません。

[English 🇬🇧](https://github.com/0Akise/Orbis/blob/master/README.md) | <ins>[日本語 🇯🇵](https://github.com/0Akise/Orbis/blob/master/README_JP.md)</ins>

- SFML を使用するにあたって自分でゼロから GUI を構築する必要なく、迅速な開発をサポートするために作られた高いカスタム性と取り扱いやすさを中心として開発を進めている 2D/3D GUI ライブラリーです。対応 C++ は 20 から、そしてライセンスは MIT を使用しています。

> [!CAUTION]
> このドキュメントは現在アップデートが必要です。

## 機能
- 💠 プロジェクトに適用しやすいヘッダーオンリーライブラリー
- 💥 C++20を使用することで現代的かつクリーンなコード
- ✅ Vulkan と SFML3 を両立することで2D/3D両方でのGUI開発をサポート
- 💚 チェーン型宣言でカスタム性の高い構造
- 💨 直観的な静的 API 構成で迅速な開発をサポート

# Requirements
- C++20 コンパイラー (MinGW)
- SFML 3.0.0
- CMake 4 以上
- Vulkan (現時点では必要ありませんが、将来のバージョンで計画されています)

SFML 固有の C++ コンパイルに関する詳細情報は [SFML3 ダウンロード](https://www.sfml-dev.org/download/sfml/3.0.0/) で確認できます。可能であれば、C++ コンパイルのために LLVM-MinGW(UCRT版) をご利用ください。詳細は[こちら](https://github.com/mstorsjo/llvm-mingw/releases)で確認できます。

# 使用例
プロジェクト内の `example` フォルダーをご確認ください。
```cpp
sf::RenderWindow window(...);
UIContext context;

UI::Initialize();
UI::Bind(window, context);

auto my_font = UI::LoadFont("./res/roboto.ttf");

auto example_button = UI::CreateWidget(WidgetType::Button);

example_button.SetSize(100, 50);

auto example_window = UI::CreatePanel();

example_window.SetName("MyWindow")
        .SetSize({400, 200})
        .SetPosition({0, screen_size.y - 200})
        .SetZLevel(1)
        .DrawRect({380, 180}, {10, 10}, 0, sf::Color({255, 255, 255, 255}))
        .DrawRect({380, 30}, {10, 10}, 1, sf::Color({0, 180, 255, 255}))
        .DrawText(*my_font, 15, {15, 15}, 20, sf::Color::White, "My Simple HUD")
        ...
        .AddWidget(example_button, {100, 100});
```
- UI クラスは静的・シングルトンパターンを使用しており、自分の作るUIがどのような動作をすべきか直接制御できるよう設計されています。
- ドットでコマンドを連鎖させることで、Panel の高いカスタマイズ性を実現します。

## インストール
これらのコードを自分のプロジェクトの `CMakeLists.txt` に書き加えます。
```cmake
include(FetchContent)
FetchContent_Declare(
    Orbis
    GIT_REPOSITORY https://github.com/0Akise/Orbis.git
    GIT_BRANCH master
)
FetchContent_MakeAvailable(Orbis)

target_link_libraries(my_project PRIVATE Orbis)
```

## ビルド
レポジトリをクローンして中に移動します。
```bash
git clone https://github.com/0Akise/Orbis.git
cd Orbis
```

次に、CMake を用いてビルドします。
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
```

例題コードの実行はビルドフォルダー直下にあります。
```bash
./build/example/{example_name}/{example_name}.exe // Windows
```

# ロードマップ
- [ ] implementation of Widget creation

# 共同開発
プロジェクト内の `CONTRIBUTION.md` ファイル(英語)をご確認ください。

# ライセンス
プロジェクト Orbis は MIT ライセンス下で使用可能です。 `LICENSE` ファイル(英語)をご確認ください。
