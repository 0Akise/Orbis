#pragma once

#include <SFML/Graphics.hpp>

namespace Orbis {
    enum class DrawingType {
        Rect,
        Text,
        Texture,
    };

    class Drawings {
    public:
        DrawingType  mType;
        std::string  mID;
        sf::Vector2f mPosition;
        size_t       mZLevel;
        sf::Color    mFillColor;

        virtual ~Drawings() = default;
    };

    class DrawingsRect : public Drawings {
    public:
        sf::Vector2f mSize;
        bool         mIsOutlined;
        float        mOutlineThickness;
        sf::Color    mOutlineColor;
        bool         mIsRounded;
        float        mRoundingRadius;
    };

    class DrawingsText : public Drawings {
    public:
        std::shared_ptr<sf::Font> mFont;
        size_t                    mFontSize;
        std::string               mText;
    };

    class DrawingsTexture : public Drawings {
    public:
        std::shared_ptr<sf::Texture> mTexture;
        sf::Vector2f                 mSize;
        bool                         mTextureSmoothing;
    };
} // namespace Orbis