#pragma once;

#include <string>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Enums.hpp"

namespace Orbis {
    class Drawings;
    class DrawingsLine;
    class DrawingsRect;
    class DrawingsText;
    class DrawingsTexture;

    class Drawings {
    public:
        DrawingType  mType;
        std::string  mID;
        sf::Vector2f mPosition;
        size_t       mZLevel;
        sf::Color    mFillColor;

        virtual ~Drawings() = default;
    };

    class DrawingsLine : public Drawings {
    public:
        std::vector<sf::Vector2f> mPoints;
        float                     mThickness;
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
        TextAlign                 mAlign;
    };

    class DrawingsTexture : public Drawings {
    public:
        std::shared_ptr<sf::Texture> mTexture;
        sf::Vector2f                 mSize;
        bool                         mTextureSmoothing;
    };
} // namespace Orbis