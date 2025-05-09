#include <Orbis/UI.hpp>
#include <Orbis/Utils.hpp>

using namespace Orbis;

// Your actual game code here, this is just an example.
struct Player {
    int mHealthMax = 100;
    int mHealthCurrent = 100;
    int mArmorMax = 100;
    int mArmorCurrent = 0;
};

struct AnimationState {
    bool mIsAnimating = false;
    std::chrono::steady_clock::time_point mStartTime;
    float mFrom = 0.0f;
    float mTo = 0.0f;
    float mCurrent = 0.0f;
    float mDuration = 0.5f;
};

int main() {
    sf::Vector2f screen_size({1080, 720});

    sf::RenderWindow window(sf::VideoMode({1080, 720}), "Orbis Examples 1: Basic HUD", sf::Style::Default);

    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);

    Player player;

    AnimationState hp_anim;
    AnimationState ap_anim;

    hp_anim.mCurrent = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
    ap_anim.mCurrent = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;

    // load your desired resources to UI so the UI class can access them.
    // If loading fails(such as wrong path), Load* functions will throw exception.
    auto& my_font = *UI::LoadFont("./res/roboto.ttf");
    auto& hp_texture = *UI::LoadTexture("./res/hp.png");
    auto& ap_texture = *UI::LoadTexture("./res/ap.png");

    // Orbis uses Chaining to create UI Widgets.
    // If you think the code might get too long, you can make separate file to store all UIs
    // in the same way described here.
    auto& frame_example = UI::Create<DFrame>();
    frame_example.SetName("MyHUD")
        .SetSize({400, 200})
        .SetPosition({0, screen_size.y - 200})
        // Be sure to set Z-Level for each Parental Widgets!
        .SetZLevel(1)
        // !! Draw functions use local position of widget !!
        .DrawRect({380, 180}, {10, 10}, 0, sf::Color({255, 255, 255, 255}))
        .DrawRect({380, 30}, {10, 10}, 1, sf::Color({0, 180, 255, 255}))
        .DrawText(my_font, 15, {15, 15}, 20, sf::Color::White, "My Simple HUD")
        .DrawTexture({32, 32}, {15, 50}, 10, sf::Color::White, hp_texture)
        .DrawTexture({32, 32}, {15, 90}, 10, sf::Color::White, ap_texture)
        .DrawRect({320, 32}, {55, 50}, 2, sf::Color({200, 200, 200, 255}))
        .DrawRect({320, 32}, {55, 90}, 2, sf::Color({200, 200, 200, 255}));

    auto& window_example1 = UI::Create<DWindow>();
    window_example1.SetName("MyPanel1")
        .SetSize({300, 300})
        .SetPosition({200, 200})
        .SetZLevel(10)
        .SetOptions(DOption::Movable);

    auto& window_example2 = UI::Create<DWindow>();
    window_example2.SetName("MyPanel2")
        .SetSize({300, 300})
        .SetPosition({600, 200})
        .SetZLevel(10)
        .SetOptions(DOption::Default);

    // notice that child of other widget uses CreateChild instead of Create function!
    // for buttons to be usable, you can set Callbacks.
    auto& button_health_up = UI::CreateChild<DButton>(window_example1);
    button_health_up.SetName("ButtonHealthUp")
        .SetSize({50, 20})
        // Child widgets follow z-level of parent widgets, so no need to set z-level unless you want to.
        .SetPosition({10, 40})
        .SetCallback([&player, &hp_anim]() {
            player.mHealthCurrent = std::min(player.mHealthMax, player.mHealthCurrent + 10);
            hp_anim.mIsAnimating = true;
            hp_anim.mStartTime = std::chrono::steady_clock::now();
            hp_anim.mFrom = hp_anim.mCurrent;
            hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
        });

    auto& button_health_down = UI::CreateChild<DButton>(window_example1);
    button_health_down.SetName("ButtonHealthDown")
        .SetSize({50, 20})
        .SetPosition({10, 80})
        .SetCallback([&player, &hp_anim]() {
            player.mHealthCurrent = std::max(0, player.mHealthCurrent - 10);
            hp_anim.mIsAnimating = true;
            hp_anim.mStartTime = std::chrono::steady_clock::now();
            hp_anim.mFrom = hp_anim.mCurrent;
            hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
        });

    auto& button_armor_up = UI::CreateChild<DButton>(window_example1);
    button_armor_up.SetName("ButtonHealthUp")
        .SetSize({50, 20})
        .SetPosition({70, 40})
        .SetCallback([&player, &ap_anim]() {
            player.mArmorCurrent = std::min(player.mArmorMax, player.mArmorCurrent + 10);
            ap_anim.mIsAnimating = true;
            ap_anim.mStartTime = std::chrono::steady_clock::now();
            ap_anim.mFrom = ap_anim.mCurrent;
            ap_anim.mTo = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;
        });

    auto& button_armor_down = UI::CreateChild<DButton>(window_example1);
    button_armor_down.SetName("ButtonHealthDown")
        .SetSize({50, 20})
        .SetPosition({70, 80})
        .SetCallback([&player, &ap_anim]() {
            player.mArmorCurrent = std::max(0, player.mArmorCurrent - 10);
            ap_anim.mIsAnimating = true;
            ap_anim.mStartTime = std::chrono::steady_clock::now();
            ap_anim.mFrom = ap_anim.mCurrent;
            ap_anim.mTo = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;
        });

    // for debugging purpose, you can list up dermas in console/terminal.
    UI::ShowDermaList();

    while (window.isOpen()) {
        // be sure to update UI before while loop usually used in SFML examples.
        // do NOT put it in event loop, Update itself has it's own loop for updating UI.
        UI::Update(window);

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
        }

        if (ap_anim.mIsAnimating == true) {
            ap_anim.mCurrent = Utils::SmoothLerp(ap_anim.mFrom, ap_anim.mTo, ap_anim.mStartTime, ap_anim.mDuration);

            if (Utils::GetTimeFactor(ap_anim.mStartTime, ap_anim.mDuration) >= 1.0f) {
                ap_anim.mIsAnimating = false;
                ap_anim.mCurrent = ap_anim.mTo;
            }
        }

        sf::Vector2f window_size_current1 = window_example1.GetSize();
        sf::Vector2f window_size_current2 = window_example2.GetSize();

        // Dynamic drawings are drawings that can be modified at the middle of the game loop.
        frame_example
            .DrawRectDynamic("frame_health", {310 * hp_anim.mCurrent, 28}, {60, 52}, 2, sf::Color({227, 47, 92, 255}))
            .DrawTextDynamic("frame_health_text", my_font, 13, {65, 55}, 20, sf::Color::White, std::to_string(player.mHealthCurrent))
            .DrawRectDynamic("frame_armor", {310 * ap_anim.mCurrent, 28}, {60, 92}, 2, sf::Color({82, 175, 255, 255}))
            .DrawTextDynamic("frame_armor_text", my_font, 13, {65, 95}, 20, sf::Color::White, std::to_string(player.mArmorCurrent));

        window_example1
            .DrawRectDynamic("window_bg1", window_size_current1, {0, 0}, 0, sf::Color::White)
            .DrawRectDynamic("window_header1", {window_size_current1.x, 30}, {0, 0}, 1, sf::Color({0, 180, 255, 255}))
            .DrawTextDynamic("window_title1", my_font, 15, {5, 5}, 20, sf::Color::White, "My Simple Window1");

        window_example2
            .DrawRectDynamic("window_bg2", window_size_current2, {0, 0}, 0, sf::Color::White)
            .DrawRectDynamic("window_header2", {window_size_current2.x, 30}, {0, 0}, 1, sf::Color({255, 180, 0, 255}))
            .DrawTextDynamic("window_title2", my_font, 15, {5, 5}, 20, sf::Color::White, "My Simple Window2");

        button_health_up
            .DrawRectDynamic("button_health_up", {50, 20}, {0, 0}, 10, sf::Color({180, 180, 180}))
            .DrawTextDynamic("button_health_up_text", my_font, 13, {5, 3}, 20, sf::Color::White, "Up");

        button_health_down
            .DrawRectDynamic("button_health_down", {50, 20}, {0, 0}, 10, sf::Color(180, 180, 180))
            .DrawTextDynamic("button_health_up_text", my_font, 13, {5, 3}, 20, sf::Color::White, "Down");

        button_armor_up
            .DrawRectDynamic("button_armor_up", {50, 20}, {0, 0}, 10, sf::Color({180, 180, 180}))
            .DrawTextDynamic("button_health_up_text", my_font, 13, {5, 3}, 20, sf::Color::White, "Up");

        button_armor_down
            .DrawRectDynamic("button_armor_down", {50, 20}, {0, 0}, 10, sf::Color(180, 180, 180))
            .DrawTextDynamic("button_health_up_text", my_font, 13, {5, 3}, 20, sf::Color::White, "Down");

        window.clear();

        // Be sure to call Render after cleaning up the window!
        UI::Render(window);

        window.display();
    }

    return 0;
}