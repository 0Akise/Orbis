#include <Orbis/UI.hpp>

using namespace Orbis;

int main() {
    sf::Vector2f     screen_size({800, 800});
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "Button Test", sf::Style::Default);

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);

    UI::Initialize();
    UIContext context = UI::CreateContext();
    UI::Bind(window, context);

    auto font_basic = UI::LoadFont("./res/roboto.ttf");

    int counter = 0;

    auto canvas_hud     = UI::CreateWidget<WidgetType::Canvas>();
    auto button_normal  = UI::CreateWidget<WidgetType::Button>();
    auto button_styled  = UI::CreateWidget<WidgetType::Button>();
    auto button_counter = UI::CreateWidget<WidgetType::Button>();

    canvas_hud
        .SetSize({400, 400})
        .SetPosition({0, 0})
        .SetZLevel(0)
        .DrawRect("background", {400, 400}, {0, 0}, 0, sf::Color(240, 240, 240, 255))
        .DrawRect("header", {400, 40}, {0, 0}, 1, sf::Color(100, 150, 255, 255))
        .DrawText("title", 20, {10, 10}, 10, sf::Color::White, font_basic, "Button Test");

    button_normal
        .SetSize({150, 50})
        .SetPosition({100, 100})
        .SetZLevel(10)
        .SetStateColor(ButtonState::Normal, sf::Color(100, 150, 255, 255))
        .SetStateColor(ButtonState::Hover, sf::Color(120, 170, 255, 255))
        .SetStateColor(ButtonState::Pressed, sf::Color(80, 130, 235, 255))
        .SetCallback([]() {
            std::cout << "Button clicked!" << std::endl;
        })
        .DrawText("btn_text", 16, {30, 15}, 10, sf::Color::White, font_basic, "Click Me!");

    button_styled
        .SetSize({150, 50})
        .SetPosition({100, 180})
        .SetZLevel(10)
        .SetStateColor(ButtonState::Normal, sf::Color(50, 200, 100, 255))
        .SetStateColor(ButtonState::Hover, sf::Color(70, 220, 120, 255))
        .SetStateColor(ButtonState::Pressed, sf::Color(30, 180, 80, 255))
        .SetCallback([]() {
            std::cout << "Styled button clicked!" << std::endl;
        })
        .DrawRect("btn_border", {146, 46}, {2, 2}, 0, sf::Color::Transparent, true, 2.0f, sf::Color::White)
        .DrawText("btn_text", 16, {25, 15}, 10, sf::Color::White, font_basic, "Styled Button");

    button_counter
        .SetSize({150, 50})
        .SetPosition({100, 260})
        .SetZLevel(10)
        .SetStateColor(ButtonState::Normal, sf::Color(255, 150, 50, 255))
        .SetStateColor(ButtonState::Hover, sf::Color(255, 170, 70, 255))
        .SetStateColor(ButtonState::Pressed, sf::Color(235, 130, 30, 255))
        .SetCallback([&counter, &button_counter, font_basic]() {
            counter++;

            std::cout << "Counter: " << counter << std::endl;

            auto& text = button_counter.GetText("btn_text");

            text.mText = "Count: " + std::to_string(counter);
        })
        .DrawText("btn_text", 16, {30, 15}, 10, sf::Color::White, font_basic, "Count: 0");

    auto panel_main = UI::CreatePanel();

    panel_main
        .SetName("ButtonTestPanel")
        .SetSize({400, 400})
        .SetPosition({10, 10})
        .SetZLevel(0)
        .AddWidget(canvas_hud)
        .AddWidget(button_normal)
        .AddWidget(button_styled)
        .AddWidget(button_counter)
        .Register(context);

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

        window.clear();

        UI::Render(window);

        window.display();
    }

    return 0;
}