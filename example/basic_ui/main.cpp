#include <Orbis/UI.hpp>
#include <Orbis/Utils.hpp>

using namespace Orbis;

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
    sf::RenderWindow window(sf::VideoMode({1080, 720}), "Orbis Examples 1: Basic HUD", sf::Style::Default);

    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);

    sf::Vector2f screen_size({1080, 720});

    Player player;

    auto& my_font = *UI::LoadFont("./res/roboto.ttf");
    auto& hp_image = *UI::LoadTexture("./res/hp.png");
    auto& ap_image = *UI::LoadTexture("./res/sp.png");

    std::string hp_id = "health";
    std::string hp_text_id = "health_text";
    std::string ap_id = "armor";
    std::string ap_text_id = "armor_text";
    AnimationState hp_anim;
    AnimationState ap_anim;

    hp_anim.mCurrent = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
    ap_anim.mCurrent = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;

    auto& example_hud = *UI::Create(DType::DFrame);
    example_hud.SetName("MainFrame")
        .SetSize({400, 200})
        .SetPosition({0, screen_size.y - 200})
        .DrawRect({380, 180}, {10, 10}, 0, sf::Color({255, 255, 255, 255}))
        .DrawRect({380, 30}, {10, 10}, 1, sf::Color({0, 180, 255, 255}))
        .DrawText(my_font, 15, {15, 15}, 20, sf::Color::White, "My Simple HUD")
        .DrawImage({32, 32}, {15, 50}, 10, sf::Color::White, hp_image)
        .DrawImage({32, 32}, {15, 90}, 10, sf::Color::White, ap_image)
        .DrawRect({320, 32}, {55, 50}, 2, sf::Color({200, 200, 200, 255}))
        .DrawRect({320, 32}, {55, 90}, 2, sf::Color({200, 200, 200, 255}));

    auto& example_window = *UI::Create(DType::DWindow);
    example_window.SetName("MainWindow")
        .SetSize({300, 300})
        .SetPosition({200, 200})
        .SetOptions(DOptionFlag::Default)
        .DrawRect({300, 300}, {0, 0}, 0, sf::Color::White)
        .DrawRect({300, 30}, {0, 0}, 1, sf::Color({0, 180, 255, 255}))
        .DrawText(my_font, 15, {5, 5}, 20, sf::Color::White, "My Simple Window");

    UI::ShowDermaList();

    while (window.isOpen()) {
        UI::Update(window);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) == true) {
                player.mHealthCurrent = std::max(0, player.mHealthCurrent - 10);
                hp_anim.mIsAnimating = true;
                hp_anim.mStartTime = std::chrono::steady_clock::now();
                hp_anim.mFrom = hp_anim.mCurrent;
                hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mHealthMax;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) == true) {
                player.mHealthCurrent = std::min(player.mHealthMax, player.mHealthCurrent + 10);
                hp_anim.mIsAnimating = true;
                hp_anim.mStartTime = std::chrono::steady_clock::now();
                hp_anim.mFrom = hp_anim.mCurrent;
                hp_anim.mTo = static_cast<float>(player.mHealthCurrent) / player.mArmorMax;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) == true) {
                player.mArmorCurrent = std::max(0, player.mArmorCurrent - 10);
                ap_anim.mIsAnimating = true;
                ap_anim.mStartTime = std::chrono::steady_clock::now();
                ap_anim.mFrom = ap_anim.mCurrent;
                ap_anim.mTo = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) == true) {
                player.mArmorCurrent = std::min(player.mArmorMax, player.mArmorCurrent + 10);
                ap_anim.mIsAnimating = true;
                ap_anim.mStartTime = std::chrono::steady_clock::now();
                ap_anim.mFrom = ap_anim.mCurrent;
                ap_anim.mTo = static_cast<float>(player.mArmorCurrent) / player.mArmorMax;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) == true) {
                std::cout << "MainWindow Size: " << example_window.GetSize().x << " " << example_window.GetSize().y << "\n";
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

        example_hud
            .DrawRectDynamic(hp_id, {310 * hp_anim.mCurrent, 28}, {60, 52}, 2, sf::Color({227, 47, 92, 255}))
            .DrawTextDynamic(hp_text_id, my_font, 13, {65, 55}, 20, sf::Color::White, std::to_string(player.mHealthCurrent))
            .DrawRectDynamic(ap_id, {310 * ap_anim.mCurrent, 28}, {60, 92}, 2, sf::Color({82, 175, 255, 255}))
            .DrawTextDynamic(ap_text_id, my_font, 13, {65, 95}, 20, sf::Color::White, std::to_string(player.mArmorCurrent));

        window.clear();

        UI::Render(window);

        window.display();
    }

    return 0;
}