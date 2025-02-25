#pragma once

#include <SFML/Graphics.hpp>

#include "orbis/controls.hpp"
#include "orbis/ui.hpp"

#include <iostream>
#include <memory>

namespace OrbisExample {
    struct ResourceVault {
        sf::Font font;

        ResourceVault() {
            if (font.openFromFile("res/font.ttf") == false) {
                throw std::runtime_error("Failed to load font");
            }
        }
    };

    class Program {
    private:
        struct M {
            ResourceVault _Vault;
            Orbis::UI _UI;
            Orbis::Controls _Controls;
        } m;

        explicit Program(M m) : m(std::move(m)) {}

    public:
        static Program Create() {
            return Program(M{
                ._Vault = ResourceVault(),
                ._UI = Orbis::UI::Create(),
                ._Controls = Orbis::Controls::Create()});
        }

        void Init() {
            m._UI.DermaRegister("Main Menu", {100, 100}, {100, 100});
            m._UI.DermaRegister("Sub Menu", {100, 100}, {200, 300});
            m._UI.DermaSetDebugMode(1, "Sub Menu", true);
            m._UI.DermaSetDraggable(1, "Sub Menu", true);
            m._UI.DermaSetResizable(1, "Sub Menu", true);
            m._UI.DermaShowList();
        }

        void Loop(sf::RenderWindow& window) {
            while (window.isOpen()) {
                while (const std::optional event = window.pollEvent()) {
                    m._Controls.SetMousePosition(sf::Mouse::getPosition(window));

                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                    }

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                        window.close();
                    }

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) == true) {
                        m._Controls.SetIsLMousePressed(true);
                    } else {
                        m._Controls.SetIsLMousePressed(false);
                    }

                    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) == true) {
                        m._Controls.SetIsRMousePressed(true);
                    } else {
                        m._Controls.SetIsRMousePressed(false);
                    }

                    m._UI.Update(m._Controls);
                }

                window.clear();

                m._UI.Render(window);

                window.display();
            }
        }
    };
}