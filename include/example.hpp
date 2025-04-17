#pragma once

#include <iostream>
#include <memory>
#include <optional>

#include <SFML/Graphics.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/UI.hpp"

namespace OrbisExample {
    struct ResourceVault {
        sf::Font font;

        ResourceVault() {
            if (font.openFromFile("res/font.ttf") == false) {
                throw std::runtime_error("Failed to load font");
            }
        }
    };
}

namespace OrbisExample {
    using namespace Orbis;

    class Program {
    private:
        ResourceVault mVault;
        Controls mControls;

    public:
        Program()
            : mVault(ResourceVault()), mControls(Controls::Create()) {}

        static Program Create() {
            return Program();
        }

        void Run(sf::RenderWindow& window) {
            sf::Vector2f screen_size({1080.0f, 720.0f});

            auto& example_hud = *UI::Create(DType::DFrame);
            example_hud.SetName("Main Frame")
                .SetSize({400.0f, 200.0f})
                .SetPosition({0.0f, screen_size.y - 200.0f})
                .DrawRect({380.0f, 180.0f}, {10.0f, 10.0f}, 0, sf::Color({255, 255, 255, 255}))
                .DrawRect({380.0f, 30.0f}, {10.0f, 10.0f}, 1, sf::Color({0, 180, 255, 255}));

            auto& example_window = *UI::Create(DType::DWindow);
            example_window.SetName("Sub Menu")
                .SetSize({300, 300})
                .SetPosition({200, 200})
                .SetDebugMode(true)
                .SetOptions(DOptionFlag::Default);

            UI::ShowDermaList();

            while (window.isOpen()) {
                mControls.SetIsLMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left));
                mControls.SetIsRMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right));
                mControls.SetIsWMousePressed(sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle));
                mControls.SetMousePosition(sf::Mouse::getPosition(window));

                while (const std::optional event = window.pollEvent()) {
                    mControls.SetMousePosition(sf::Mouse::getPosition(window));

                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                    }

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                        window.close();
                    }
                }

                UI::Update(mControls);

                window.clear();

                UI::Render(window);

                window.display();
            }
        }
    };
}