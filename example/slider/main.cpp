#include <Orbis/UI.hpp>

using namespace Orbis;

int main() {
    sf::Vector2f     screen_size({800, 800});
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "Slider Test", sf::Style::Default);

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);

    UI::Initialize();
    UIContext context = UI::CreateContext();
    UI::Bind(window, context);

    auto font = UI::LoadFont("./res/roboto.ttf");

    sf::Color mixed_color = sf::Color(128, 128, 128, 255);

    auto canvas_background    = UI::CreateWidget<WidgetType::Canvas>();
    auto canvas_color_preview = UI::CreateWidget<WidgetType::Canvas>();
    auto slider_red           = UI::CreateWidget<WidgetType::Slider>();
    auto slider_green         = UI::CreateWidget<WidgetType::Slider>();
    auto slider_blue          = UI::CreateWidget<WidgetType::Slider>();
    auto label_title          = UI::CreateWidget<WidgetType::Canvas>();

    canvas_background
        .SetSize({360, 130})
        .SetPosition({0, 0})
        .DrawRect("background", {360, 130}, {0, 0}, 0, sf::Color(50, 50, 50, 255), true);

    canvas_color_preview
        .SetSize({100, 100})
        .SetPosition({10, 10})
        .DrawRect("preview", {100, 100}, {0, 0}, 0, mixed_color, true, 2.0f, sf::Color::Black);

    slider_red
        .SetSize({150, 20})
        .SetPosition({130, 20})
        .SetRange(0.0f, 255.0f)
        .SetValue(128.0f)
        .SetStepSize(1.0f)
        .SetFillColor(sf::Color(255, 100, 100, 255))
        .SetCallback([&mixed_color, &canvas_color_preview](float value) {
            mixed_color.r      = static_cast<std::uint8_t>(value);
            auto& preview      = canvas_color_preview.GetRect("preview");
            preview.mFillColor = mixed_color;
        });

    slider_green
        .SetSize({150, 20})
        .SetPosition({130, 50})
        .SetRange(0.0f, 255.0f)
        .SetValue(128.0f)
        .SetStepSize(1.0f)
        .SetFillColor(sf::Color(100, 255, 100, 255))
        .SetCallback([&mixed_color, &canvas_color_preview](float value) {
            mixed_color.g      = static_cast<std::uint8_t>(value);
            auto& preview      = canvas_color_preview.GetRect("preview");
            preview.mFillColor = mixed_color;
        });

    slider_blue
        .SetSize({150, 20})
        .SetPosition({130, 80})
        .SetRange(0.0f, 255.0f)
        .SetValue(128.0f)
        .SetStepSize(1.0f)
        .SetFillColor(sf::Color(100, 100, 255, 255))
        .SetCallback([&mixed_color, &canvas_color_preview](float value) {
            mixed_color.b      = static_cast<std::uint8_t>(value);
            auto& preview      = canvas_color_preview.GetRect("preview");
            preview.mFillColor = mixed_color;
        });

    label_title
        .SetSize({200, 30})
        .SetPosition({10, 100})
        .DrawText("label_title", 14, {0, 15}, 0, sf::Color::White, font, TextAlign::LeftCenter, "RGB Color Mixer");

    auto panel_main = UI::CreatePanel();

    panel_main
        .SetName("SliderExamples")
        .SetSize({400, 300})
        .SetPosition({10, 10})
        .SetZLevel(0)
        .AddWidget(canvas_background)
        .AddWidget(canvas_color_preview)
        .AddWidget(slider_red)
        .AddWidget(slider_green)
        .AddWidget(slider_blue)
        .AddWidget(label_title)
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
