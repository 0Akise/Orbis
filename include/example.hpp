#pragma once

#include <iostream>
#include <memory>
#include <optional>

#include <SFML/Graphics.hpp>

#include "orbis/controls.hpp"
#include "orbis/ui.hpp"

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
            Derma& example_frame = UI::Create(DermaType::DFrame);
            example_frame.SetName("Main Frame")
                .SetSize({100, 100})
                .SetPosition({100, 100});

            Derma& example_window = UI::Create(DermaType::DWindow);
            example_window.SetName("Sub Menu")
                .SetSize({300, 300})
                .SetPosition({200, 200})
                .SetDebugMode(true)
                .SetComponents(DermaComponentFlag::Complete);

            UI::ShowDermaList();

            while (window.isOpen()) {
                while (const std::optional event = window.pollEvent()) {
                    mControls.SetMousePosition(sf::Mouse::getPosition(window));

                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                    }

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                        window.close();
                    }

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) == true) {
                        mControls.SetIsLMousePressed(true);
                    } else {
                        mControls.SetIsLMousePressed(false);
                    }

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) == true) {
                        mControls.SetIsRMousePressed(true);
                    } else {
                        mControls.SetIsRMousePressed(false);
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