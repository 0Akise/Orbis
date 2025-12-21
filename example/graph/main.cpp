#include <Orbis/UI.hpp>
#include <Orbis/Utils.hpp>

using namespace Orbis;

int main() {
    sf::Vector2f     screen_size({800, 800});
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "Graph Test", sf::Style::Default);

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);

    UI::Initialize();

    UIContext context = UI::CreateContext();
    UI::Bind(window, context);

    auto font = UI::LoadFont("./res/roboto.ttf");

    auto  points          = Utils::GeneratePoints([](float x) { return (x * x * x) + (x * x) - (3.0f * x) + 4.0f; }, -10.0f, 10.0f, 0.05f, 40.0f, 40.0f, {400, 400});
    float screen_center_x = screen_size.x / 2.0f;
    float screen_center_y = screen_size.y / 2.0f;

    auto canvas_field = UI::CreateWidget<WidgetType::Canvas>();

    canvas_field
        .SetSize({800, 800})
        .SetPosition({0, 0})
        .DrawRect("background", {800, 800}, {0, 0}, 0, sf::Color(30, 30, 30, 255));

    for (int i = 1; i <= 10; ++i) {
        float position = static_cast<float>(i) * 80.0f;

        canvas_field
            .DrawRect("hline_" + std::to_string(i), {screen_size.x, 2.0f}, {0.0f, position - 1.0f}, 1, sf::Color(50, 50, 50, 255))
            .DrawRect("vline_" + std::to_string(i), {2.0f, screen_size.y}, {position - 1.0f, 0.0f}, 1, sf::Color(50, 50, 50, 255));
    }

    canvas_field
        .DrawRect("axis_x", {screen_size.x, 3.0f}, {0.0f, screen_center_y - 1.5f}, 2, sf::Color(150, 150, 150, 255))
        .DrawRect("axis_y", {3.0f, screen_size.y}, {screen_center_x - 1.5f, 0.0f}, 2, sf::Color(150, 150, 150, 255));

    auto panel_main = UI::CreatePanel();

    panel_main
        .SetName("GraphExample")
        .SetSize({800, 800})
        .SetPosition({0, 0})
        .SetZLevel(0)
        .AddWidget(canvas_field)
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

        canvas_field
            .DrawLine("graph_curve", points, 3, sf::Color(200, 100, 100, 255), 4.0f);

        UI::Render(window);

        window.display();
    }

    return 0;
}