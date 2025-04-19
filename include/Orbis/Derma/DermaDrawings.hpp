#pragma once

#include <SFML/Graphics.hpp>

#include "Orbis/Derma/DermaEnums.hpp"

namespace Orbis {
    class DermaDrawings {
    public:
        DDrawingsType mType;
        std::string mID;
        sf::Vector2f mPosition;
        size_t mZLevel;
        sf::Color mFillColor;

        virtual ~DermaDrawings() = default;
    };

    class DrawingsRect : public DermaDrawings {
    public:
        sf::Vector2f mSize;

        bool mIsOutlined;
        float mOutlineThickness;
        sf::Color mOutlineColor;

        bool mIsRounded;
        float mRoundingRadius;
    };

    class DrawingsText : public DermaDrawings {
    public:
        sf::Font mFont;
        size_t mFontSize;
        std::string mText;
    };

    class DrawingsTexture : public DermaDrawings {
    public:
        sf::Vector2f mSize;

        sf::Texture mTexture;
        bool mTextureSmoothing;
    };
}