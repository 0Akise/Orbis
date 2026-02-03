#pragma once;

#include <map>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Controls.hpp"
#include "Orbis/System/Drawings.hpp"

namespace Orbis {
    class Widget;
    class Canvas;
    class Button;
    class Slider;
    class TextboxSingle;
    class TextboxMultiLine;
    class Panel;

    class Widget : public std::enable_shared_from_this<Widget> {
    protected:
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::map<std::string, std::shared_ptr<DrawingsLine>>    mDrawingsLine;
        std::map<std::string, std::shared_ptr<DrawingsRect>>    mDrawingsRect;
        std::map<std::string, std::shared_ptr<DrawingsText>>    mDrawingsText;
        std::map<std::string, std::shared_ptr<DrawingsTexture>> mDrawingsTexture;

    public:
        virtual ~Widget() = default;

        DrawingsRect& GetRect(const std::string& id) {
            auto iter = mDrawingsRect.find(id);

            if (iter == mDrawingsRect.end()) {
                throw std::runtime_error("DrawingsRect with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsText& GetText(const std::string& id) {
            auto iter = mDrawingsText.find(id);

            if (iter == mDrawingsText.end()) {
                throw std::runtime_error("DrawingsText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsTexture& GetTexture(const std::string& id) {
            auto iter = mDrawingsTexture.find(id);

            if (iter == mDrawingsTexture.end()) {
                throw std::runtime_error("DrawingsTexture with id '" + id + "' not found");
            }

            return *iter->second;
        }

        sf::Vector2f GetPosition() const {
            return mPosition;
        }

        sf::Vector2f GetSize() const {
            return mSize;
        }

        size_t GetZLevel() const {
            return mZLevel;
        }

        bool GetVisibility() const {
            return mIsVisible;
        }

        Widget& SetSize(sf::Vector2f size) {
            mSize = size;

            return *this;
        }

        Widget& SetPosition(sf::Vector2f position) {
            mPosition = position;

            return *this;
        }

        Widget& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;

            return *this;
        }

        Widget& SetVisibility(bool visible) {
            mIsVisible = visible;

            return *this;
        }

        Widget& DrawLine(
            const std::string&               id,
            const std::vector<sf::Vector2f>& points,
            size_t                           zlevel,
            sf::Color                        color,
            float                            thickness = 2.0f) {
            auto drawing = std::make_shared<DrawingsLine>();

            drawing->mType      = DrawingType::Line;
            drawing->mID        = id;
            drawing->mPoints    = points;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = color;
            drawing->mThickness = thickness;

            mDrawingsLine[id] = drawing;

            return *this;
        }

        Widget& DrawRect(
            const std::string& id,
            sf::Vector2f       size,
            sf::Vector2f       position,
            size_t             zlevel,
            sf::Color          fill_color,
            bool               is_outlined       = false,
            float              outline_thickness = 0.0f,
            sf::Color          outline_color     = sf::Color::Black,
            bool               is_rounded        = false,
            float              rounding_radius   = 0.0f) {
            auto drawing = std::make_shared<DrawingsRect>();

            drawing->mType             = DrawingType::Rect;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mIsOutlined       = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor     = outline_color;
            drawing->mIsRounded        = is_rounded;
            drawing->mRoundingRadius   = rounding_radius;

            mDrawingsRect[id] = drawing;

            return *this;
        }

        Widget& DrawText(
            const std::string&        id,
            size_t                    font_size,
            sf::Vector2f              position,
            size_t                    zlevel,
            sf::Color                 fill_color,
            std::shared_ptr<sf::Font> font,
            TextAlign                 align = TextAlign::LeftTop,
            const std::string&        text  = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType      = DrawingType::Text;
            drawing->mID        = id;
            drawing->mFontSize  = font_size;
            drawing->mPosition  = position;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFont      = font;
            drawing->mAlign     = align;
            drawing->mText      = text;

            mDrawingsText[id] = drawing;

            return *this;
        }

        Widget& DrawTexture(
            const std::string&           id,
            sf::Vector2f                 size,
            sf::Vector2f                 position,
            size_t                       zlevel,
            sf::Color                    fill_color,
            std::shared_ptr<sf::Texture> texture,
            bool                         smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType             = DrawingType::Texture;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mTexture          = texture;
            drawing->mTextureSmoothing = smoothing_enabled;

            mDrawingsTexture[id] = drawing;

            return *this;
        }

        virtual std::shared_ptr<Widget> CloneImpl() const                                            = 0;
        virtual void                    UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) = 0;
        virtual void                    RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) = 0;
    };
} // namespace Orbis