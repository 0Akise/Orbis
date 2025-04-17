#include "Orbis/Controls.hpp"
#include "Orbis/ResourceVault.hpp"
#include "Orbis/UI.hpp"

using namespace Orbis;

int main() {
    sf::RenderWindow window(sf::VideoMode({1080, 720}), "Orbis Examples", sf::Style::Default);

    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);

    ResourceVault resvault;
    Controls controls = Controls::Create();
    sf::Vector2f screen_size({1080.0f, 720.0f});
    std::string font_path = "./res/font.ttf";

    auto& example_hud = *UI::Create(DType::DFrame);
    example_hud.SetName("Main Frame")
        .SetSize({400.0f, 200.0f})
        .SetPosition({0.0f, screen_size.y - 200.0f})
        .DrawRect({380.0f, 180.0f}, {10.0f, 10.0f}, 0, sf::Color({255, 255, 255, 255}))
        .DrawRect({380.0f, 30.0f}, {10.0f, 10.0f}, 1, sf::Color({0, 180, 255, 255}))
        .DrawText(font_path, 15, {15.0f, 15.0f}, 100, sf::Color::White, "My HUD");

    auto& example_window = *UI::Create(DType::DWindow);
    example_window.SetName("Sub Menu")
        .SetSize({300, 300})
        .SetPosition({200, 200})
        .SetDebugMode(true)
        .SetOptions(DOptionFlag::Default);

    UI::ShowDermaList();

    while (window.isOpen()) {
        controls.SetMousePosition(sf::Mouse::getPosition(window));
        controls.SetIsLMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left));
        controls.SetIsRMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right));
        controls.SetIsWMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle));

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                window.close();
            }
        }

        UI::Update(controls);

        window.clear();

        UI::Render(window);

        window.display();
    }

    return 0;
}