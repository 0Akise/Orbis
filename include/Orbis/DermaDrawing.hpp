#pragma once

#include <SFML/Graphics.hpp>

#include "Orbis/Data.hpp"

namespace Orbis {
    class DermaDrawing {
    public:
        DDrawingType mType;

        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;
        sf::Color mFillColor;

        bool mIsOutlined;
        float mOutlineThickness;
        sf::Color mOutlineColor;
        
        bool mIsRounded;
        float mRoundingRadius;
    };
}