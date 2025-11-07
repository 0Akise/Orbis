// WARNING: Currently working on massive migration!
// THIS CODE IS A CONCEPT FOR NOW, NOT A WORKING EXAMPLE!

#include <iostream>

#include <Orbis/System/Utils.hpp>
#include <Orbis/UI.hpp>

using namespace Orbis;

// Your actual game code here, this is just an example.
struct Player {
    int mHealthMax = 100;
    int mHealthCurrent = 100;
};

struct AnimationState {
    bool mIsAnimating = false;
    std::chrono::steady_clock::time_point mStartTime;
    float mFrom = 0.0f;
    float mTo = 0.0f;
    float mCurrent = 0.0f;
    float mDuration = 0.5f;
};

// more game code...

// SFML Entrance
int main() {
    sf::Vector2f screen_size({1080, 720});

    // Create Game window and UI context. You can have multiple windows and contexts!
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "Orbis Examples: Basics", sf::Style::Default);
    UIContext context;

    // Settings for SFML game window
    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);

    // Don't forget to initialize UI system to create UI cache!
    // Now you can bind each UI context to the window you want.
    UI::Initialize();
    UI::Bind(window, context);

    // Some game code you desire. this is an example.
    // Orbis provides some utility functions for UI making such as smooth lerp.
    Player player;
    AnimationState hp_anim;

    hp_anim.mCurrent = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;

    // more game codes...

    // load your desired resources to UI cache so you can reuse them across different UI contexts.
    // If loading fails(such as wrong path), Load* functions will throw exception.
    auto my_font = UI::LoadFont("./res/roboto.ttf");
    auto hp_texture = UI::LoadTexture("./res/hp.png");

    // Orbis uses Dot Chaining to create UI Panels and Widgets.
    // They are cached just like fonts/textures, and can be reused to multiple context.
    // You can move these UI code into separated .cpp file if it gets too long.
    auto widget_hud = UI::CreateWidget(WidgetType::Canvas);
    auto widget_button_hp_up = UI::CreateWidget(WidgetType::Button);
    auto widget_button_hp_down = UI::CreateWidget(WidgetType::Button);
    auto widget_button_exit = UI::CreateWidget(WidgetType::Button);

    // Here, we first declare STATIC stuffs that doesn't change over time.
    widget_hud
        .SetSize({400, 250})
        .SetPosition({0, 0})
        .DrawRect({380, 180}, {0, 0}, 0, sf::Color({255, 255, 255, 255}))         // background
        .DrawRect({380, 30}, {0, 0}, 1, sf::Color({0, 180, 255, 255}))            // upper strip
        .DrawText(*my_font, 15, {15, 15}, 20, sf::Color::White, "My Simple HUD")  // upper strip text
        .DrawRect({320, 32}, {55, 50}, 2, sf::Color({200, 200, 200, 255}))        // HP bar background
        .DrawTexture({32, 32}, {15, 50}, 10, sf::Color::White, *hp_texture);      // HP bar icon

    // things like HUD need to change dynamically as the game progresses.
    // so we declare DYNAMIC stuffs that changes over time!
    // be sure to use auto& instead of auto for dynamic parts.
    auto& hud_hp_bar = widget_hud.DrawRect({310, 28}, {60, 52}, 3, sf::Color({227, 47, 92, 255}));
    auto& hud_hp_text = widget_hud.DrawText(*my_font, 13, {65, 55}, 20, sf::Color::White, "");

    widget_button_hp_up
        .SetSize({100, 50})
        .SetPosition({0, 0})
        .SetCallback([&player, &hp_anim]() {
            player.mHealthCurrent = std::min(player.mHealthMax, player.mHealthCurrent + 10);
            hp_anim.mIsAnimating = true;
            hp_anim.mStartTime = std::chrono::steady_clock::now();
            hp_anim.mFrom = hp_anim.mCurrent;
            hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
        });

    widget_button_hp_down
        .SetSize({100, 50})
        .SetPosition({0, 0})
        .SetCallback([&player, &hp_anim]() {
            player.mHealthCurrent = std::max(0, player.mHealthCurrent - 10);
            hp_anim.mIsAnimating = true;
            hp_anim.mStartTime = std::chrono::steady_clock::now();
            hp_anim.mFrom = hp_anim.mCurrent;
            hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
        });

    widget_button_exit
        .SetSize({50, 50})
        .SetPosition({0, 0})
        .SetCallback([]() {
            // some code...
        });

    // Now we create Panels which you can add widgets we created previously!
    auto some_hud = UI::CreatePanel();
    auto menu_window = UI::CreatePanel();
    auto exit_window = UI::CreatePanel();

    // you can add as many widgets as possible to a panel by using dot chaining!
    // you can register your panel to certain context, and you can stack Register() too!
    some_hud
        .SetName("SomeHUD")
        .SetSize({400, 200})
        .SetPosition({0, screen_size.y - 200})
        .SetZLevel(1)
        .AddWidget(widget_hud)
        .Register(context);
    // .Register(context2);

    menu_window
        .SetName("MenuWindow")
        .SetSize({400, 400})
        .SetPosition({100, 100})
        .SetZLevel(10)
        .AddWidget(widget_button_hp_up)
        .AddWidget(widget_button_hp_down)
        .Register(context);

    exit_window
        .SetName("ExitWindow")
        .SetSize({400, 250})
        .SetPosition({200, 200})
        .SetZLevel(1000)
        .AddWidget(widget_button_exit)
        .Register(context);

    // for debugging purpose, you can list up panels in console/terminal.
    UI::ShowPanelList();

    // SFML Main Game Loop
    while (window.isOpen()) {
        // Be sure to update the UI at the START of the game loop.
        // NOTICE - UI::Update() and UI::Render() uses window, not a context!
        UI::Update(window);

        // SFML Game Event Loop
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                window.close();
            }
        }

        if (hp_anim.mIsAnimating == true) {
            hp_anim.mCurrent = Utils::SmoothLerp(hp_anim.mFrom, hp_anim.mTo, hp_anim.mStartTime, hp_anim.mDuration);

            if (Utils::GetTimeFactor(hp_anim.mStartTime, hp_anim.mDuration) >= 1.0f) {
                hp_anim.mIsAnimating = false;
                hp_anim.mCurrent = hp_anim.mTo;
            }

            // dynamic elements can be updated IN the game loop!
            hud_hp_bar.SetSize({310 * hp_anim.mCurrent, 28});
            hud_hp_text.SetText(std::to_string(player.mHealthCurrent));
        }

        // Be sure to call UI::Render() after cleaning up the window!
        window.clear();

        UI::Render(window);

        window.display();
    }

    return 0;
}