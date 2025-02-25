#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace sf {
    sf::VertexArray RectWireframe(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color) {
        sf::VertexArray vertices(sf::PrimitiveType::LineStrip, 6);
        sf::Vector2f corners[4] = {
            pos,
            {pos.x, pos.y + size.y},
            {pos.x + size.x, pos.y + size.y},
            {pos.x + size.x, pos.y}};

        for (int i = 0; i < 6; i++) {
            if (i % 4 == 0)
                vertices[i].position = corners[0];
            else
                vertices[i].position = corners[i];
        }

        for (int i = 0; i < 5; ++i) {
            vertices[i].color = color;
        }
        return vertices;
    }
}
