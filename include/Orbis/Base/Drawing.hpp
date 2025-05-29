#pragma once

#include <SFML/Graphics.hpp>

namespace Orbis {
    enum class DrawingType {
        Rect,
        Text,
        Texture,
    };

    class Drawing {
    public:
        DrawingType mType;
        std::string mID;
        sf::Vector2f mPosition;
        size_t mZLevel;
        sf::Color mFillColor;

        virtual ~Drawing() = default;
    };

    class DrawingRect : public Drawing {
    public:
        sf::Vector2f mSize;

        bool mIsOutlined;
        float mOutlineThickness;
        sf::Color mOutlineColor;

        bool mIsRounded;
        float mRoundingRadius;
    };

    class DrawingText : public Drawing {
    public:
        sf::Font mFont;
        size_t mFontSize;
        std::string mText;
    };

    class DrawingTexture : public Drawing {
    public:
        sf::Vector2f mSize;

        sf::Texture mTexture;
        bool mTextureSmoothing;
    };
}