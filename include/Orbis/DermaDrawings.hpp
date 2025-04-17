#pragma once

#include <SFML/Graphics.hpp>

#include "Orbis/Data.hpp"

namespace Orbis {
    class DermaDrawings {
    public:
        DDrawingsType mType;

        virtual ~DermaDrawings() = default;
    };

    class DrawingsRect : DermaDrawings {
    public:
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

    class DrawingsText : DermaDrawings {
    public:
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;
        sf::Color mFillColor;

        size_t mFontSize;
        std::string mText;
    };
}