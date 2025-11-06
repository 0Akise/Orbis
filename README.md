# 𓆩⚚𓆪 Orbis
> [!CAUTION]
> Orbis is in a VERY EARLY stage of development. DO NOT use it for production-ready applications.

<ins>[English 🇬🇧](https://github.com/0Akise/Orbis/blob/master/README.md)</ins> | [日本語 🇯🇵](https://github.com/0Akise/Orbis/blob/master/README_JP.md)

![grafik](https://github.com/user-attachments/assets/b1c4275b-0c8a-47dc-a6ef-ea0b1547d495)

(screenshot of `basic_ui` example)

- A modern C++20 UI library built on SFML3, offering a highly customizable yet user-friendly interface for creating both 2D and 3D GUIs.
- Orbis provides support for developers who want to build applications and games with SFML without having to develop their own GUI system from scratch.

## Features
- 💠 Header-only library for seamless integration
- 💥 Modern C++20 features for clean, efficient code
- ✅ Built on SFML3 with Vulkan support
- 💚 Highly customizable UI Components
- 💨 Intuitive API for rapid development

# Requirements
- C++20 compiler (MinGW)
- SFML 3.0.0
- CMake 4 or higher
- Vulkan (not required for now, but planned for future versions)

for further information about SFML-Specific C++ compiling, check [SFML3 Download Link](https://www.sfml-dev.org/download/sfml/3.0.0/).
If possible, use LLVM-MinGW with UCRT which you can download from [Here](https://github.com/mstorsjo/llvm-mingw/releases).

# Usage
Check out `example` folder for further information.
```cpp
sf::RenderWindow window(sf::VideoMode(...), "...", sf::Style::Default);
UIContext context;

UI::Initialize();
UI::Bind(window, context);

auto my_font = UI::LoadFont(context, "./res/roboto.ttf");

auto example_button = UI::CreateWidget(WidgetType::Button);
example_button.SetSize(100, 50);

auto example_window = UI::CreateDerma(context);
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
- UI Class uses static/singleton pattern, which gives you direct control of how the UI should work.
- by chaining commands with dots, you can control your Derma components with high customizability.

## Install
add the follwing lines to your project's `CMakeLists.txt`:
```bash
include(FetchContent)
FetchContent_Declare(
    Orbis
    GIT_REPOSITORY https://github.com/0Akise/Orbis.git
    GIT_BRANCH master
)
FetchContent_MakeAvailable(Orbis)

target_link_libraries(my_project PRIVATE Orbis)
```

## Build
clone the repository and move into it:
```bash
git clone https://github.com/0Akise/Orbis.git
cd Orbis
```

and then build with CMake:
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
```

If you want to try out examples, run with following:
```bash
./build/example/{example_name}/{example_name}.exe // Windows
```

# Roadmap
- [ ] implementation of Widget creation

# Contribution
Please check `CONTRIBUTION.md` for guidelines and information on how to contribute to the project.

# License
Orbis is available under MIT License. See `LICENSE` for details.
