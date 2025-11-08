#include <Orbis/UINew.hpp>

using namespace Orbis;

int main() {
    sf::Vector2f     screen_size({800, 600});
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "UINew Test", sf::Style::Default);

    window.setFramerateLimit(75);
    window.setVerticalSyncEnabled(false);

    UI::Initialize();
    UIContext context = UI::CreateContext();
    UI::Bind(window, context);

    auto font_basic = UI::LoadFont("./res/roboto.ttf");

    auto canvas_test = std::make_shared<Canvas>();

    canvas_test->SetSize({400, 300})
        .SetPosition({50, 50})
        .DrawRect("background", {380, 280}, {10, 10}, 0, sf::Color(240, 240, 240, 255))
        .DrawRect("header", {380, 40}, {10, 10}, 1, sf::Color(100, 150, 255, 255))
        .DrawText("title", font_basic, 20, {20, 20}, 10, sf::Color::White, "UINew Test")
        .DrawRect("static_box", {100, 100}, {50, 80}, 2, sf::Color(255, 100, 100, 255))
        .DrawText("info", font_basic, 14, {20, 200}, 10, sf::Color::Black, "Static Elements");

    auto& dynamic_text = canvas_test->GetText("title");

    auto panel_main = std::make_shared<Panel>();

    panel_main->SetName("MainPanel")
        .SetSize({500, 400})
        .SetPosition({100, 100})
        .SetZLevel(1)
        .AddWidget(*canvas_test)
        .Register(context);

    int frame_count = 0;

    while (window.isOpen()) {
        UI::Update(window);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                window.close();
            }
        }

        frame_count++;

        if ((frame_count % 60) == 0) {
            int seconds = frame_count / 60;

            dynamic_text.mText = "UINew Test - " + std::to_string(seconds) + "s";
        }

        window.clear(sf::Color(50, 50, 50, 255));
        UI::Render(window);
        window.display();
    }

    return 0;
}