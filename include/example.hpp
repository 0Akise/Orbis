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

        void Init() {
            Derma& my_window = UI::Create(DermaType::DWindow);
            my_window.SetName("Sub Menu")
                .SetSize({100, 100})
                .SetPosition({200, 200})
                .SetDebugMode(true)
                .SetComponents(DermaComponentFlag::Complete);

            UI::ShowDermaList();
        }

        void Loop(sf::RenderWindow& window) {
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