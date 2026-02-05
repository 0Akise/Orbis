#pragma once

#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace sf {
    sf::VertexArray RectWireframe(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color) {
        sf::VertexArray vertices(sf::PrimitiveType::LineStrip, 6);
        sf::Vector2f    corners[4] = {
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

    sf::ConvexShape RectRounded(const sf::Vector2f& size, float radius, size_t corner_segments = 10) {
        const float PI         = 3.1415;
        float       radius_max = std::min(size.x, size.y) / 2.0f;

        radius = std::min(radius, radius_max);

        size_t          points_total = corner_segments * 4;
        float           angle_step   = ((PI / 2.0f) / static_cast<float>(corner_segments));
        size_t          index        = 0;
        sf::ConvexShape shape(points_total);

        for (size_t i = 0; i < corner_segments; ++i) {
            float angle = ((-PI / 2.0f) + (static_cast<float>(i) * angle_step));
            float x     = (size.x - radius) + (radius * std::cos(angle));
            float y     = radius + (radius * std::sin(angle));

            shape.setPoint(index++, {x, y});
        }

        for (size_t i = 0; i < corner_segments; ++i) {
            float angle = (0 + static_cast<float>(i) * angle_step);
            float x     = (size.x - radius) + (radius * std::cos(angle));
            float y     = (size.y - radius) + (radius * std::sin(angle));

            shape.setPoint(index++, {x, y});
        }

        for (size_t i = 0; i < corner_segments; ++i) {
            float angle = ((PI / 2.0f) + (static_cast<float>(i) * angle_step));
            float x     = radius + (radius * std::cos(angle));
            float y     = (size.y - radius) + (radius * std::sin(angle));

            shape.setPoint(index++, {x, y});
        }

        for (size_t i = 0; i < corner_segments; ++i) {
            float angle = (PI + (static_cast<float>(i) * angle_step));
            float x     = radius + (radius * std::cos(angle));
            float y     = radius + (radius * std::sin(angle));

            shape.setPoint(index++, {x, y});
        }

        return shape;
    }
} // namespace sf
