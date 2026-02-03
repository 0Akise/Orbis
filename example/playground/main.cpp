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

    while (window.isOpen() == true) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            UI::ProcessEvent(window, *event);
        }

        UI::Update(window);

        window.clear();

        UI::Render(window);

        window.display();
    }

    return 0;
}
