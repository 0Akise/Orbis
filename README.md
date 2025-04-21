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

## What's Derma?
_Derma_ means "Skin" in ancient Greek (the same _derma_ as in _dermatologist_). Think of it like weapon skins in games - you have a base weapon that can be decorated with various designs.
In the same way, Orbis's Derma system lets you build interactive GUIs with your design choices quickly and reliably. Create the structure once, then customize the appearance as needed.

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
// Creates Frame you can draw and customize, such as HUD for your game.
auto& frame = *UI::Create(DType::DFrame);
frame.SetName("Main Frame")
    .SetSize({400.0f, 200.0f})
    .SetPosition({10.0f, 10.0f})
    .DrawRect({380.0f, 180.0f}, {10.0f, 10.0f}, 0, sf::Color::White);

UI::ShowDermaList();
// in main game loop
UI::Update(window);
UI::Render(window);
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
- [x] implementation of DButton

# Contribution
Please check `CONTRIBUTION.md` for guidelines and information on how to contribute to the project.

# License
Orbis is available under MIT License. See `LICENSE` for details.
