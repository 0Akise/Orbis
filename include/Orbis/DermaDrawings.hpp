#pragma once

#include <SFML/Graphics.hpp>

#include "Orbis/Data.hpp"

namespace Orbis {
class DermaDrawings {
public:
    DDrawingsType mType;
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
    std::string mFontPath;
    size_t mFontSize;
    std::string mText;
};
}