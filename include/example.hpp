#pragma once

#include <iostream>
#include <memory>

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
    class Program {
    private:
        ResourceVault mVault;
        Orbis::UI mUI;
        Orbis::Controls mControls;

    public:
        Program()
            : mVault(ResourceVault()), mUI(Orbis::UI::Create()), mControls(Orbis::Controls::Create()) {}

        static Program Create() {
            return Program();
        }

        void Init() {
            mUI.DermaRegister("Main Menu", {100, 100}, {100, 100});
            mUI.DermaRegister("Sub Menu", {100, 100}, {200, 300});
            mUI.DermaSetDebugMode(1, "Sub Menu", true);
            mUI.DermaSetSelectable(1, "Sub Menu", true);
            mUI.DermaSetMovable(1, "Sub Menu", true);
            mUI.DermaSetResizable(1, "Sub Menu", true);
            mUI.DermaShowList();
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

                    mUI.Update(mControls);
                }

                window.clear();

                mUI.Render(window);

                window.display();
            }
        }
    };
}