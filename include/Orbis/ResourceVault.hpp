#pragma once

#include <SFML/Graphics.hpp>

namespace Orbis {
    class ResourceVault {
    public:
        sf::Font font;

        ResourceVault() {
            if (font.openFromFile("res/font.ttf") == false) {
                throw std::runtime_error("Failed to load font");
            }
        }
    };
}