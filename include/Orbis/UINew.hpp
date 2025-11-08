#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Controls.hpp"
#include "Orbis/System/Drawing.hpp"
#include "Orbis/System/ResourceVault.hpp"

namespace Orbis {
    enum class WidgetType {
        Canvas,
        Button,
    };

    enum class ButtonState {
        Normal,
        Hover,
        Pressed,
    };

    class UIContext;

    class Widget : public std::enable_shared_from_this<Widget> {
    protected:
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        bool         mIsVisible = true;

        std::map<std::string, std::shared_ptr<DrawingsRect>>    mDrawingRects;
        std::map<std::string, std::shared_ptr<DrawingsText>>    mDrawingTexts;
        std::map<std::string, std::shared_ptr<DrawingsTexture>> mDrawingTextures;

    public:
        virtual ~Widget() = default;

        DrawingsRect& GetRect(const std::string& id) {
            auto iter = mDrawingRects.find(id);

            if (iter == mDrawingRects.end()) {
                throw std::runtime_error("DrawingsRect with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsText& GetText(const std::string& id) {
            auto iter = mDrawingTexts.find(id);

            if (iter == mDrawingTexts.end()) {
                throw std::runtime_error("DrawingsText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsTexture& GetTexture(const std::string& id) {
            auto iter = mDrawingTextures.find(id);

            if (iter == mDrawingTextures.end()) {
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

        bool IsVisible() const {
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

        Widget& SetVisible(bool visible) {
            mIsVisible = visible;

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

            mDrawingRects[id] = drawing;

            return *this;
        }

        Widget& DrawText(
            const std::string&        id,
            std::shared_ptr<sf::Font> font,
            size_t                    font_size,
            sf::Vector2f              position,
            size_t                    zlevel,
            sf::Color                 fill_color,
            const std::string&        text = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType      = DrawingType::Text;
            drawing->mID        = id;
            drawing->mFont      = font;
            drawing->mFontSize  = font_size;
            drawing->mPosition  = position;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mText      = text;

            mDrawingTexts[id] = drawing;

            return *this;
        }

        Widget& DrawTexture(
            const std::string&           id,
            std::shared_ptr<sf::Texture> texture,
            sf::Vector2f                 size,
            sf::Vector2f                 position,
            size_t                       zlevel,
            sf::Color                    fill_color,
            bool                         smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType             = DrawingType::Texture;
            drawing->mTexture          = texture;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mTextureSmoothing = smoothing_enabled;

            mDrawingTextures[id] = drawing;

            return *this;
        }

        virtual void Update(const Controls& controls, sf::Vector2f pos_panel) = 0;
        virtual void Render(sf::RenderWindow& window, sf::Vector2f pos_panel) = 0;
    };

    class Canvas : public Widget {
    private:
        void RenderDrawing(sf::RenderWindow&                window,
                           const std::shared_ptr<Drawings>& drawing,
                           sf::Vector2f                     pos_widget) {
            sf::Vector2f pos_drawing = pos_widget + (drawing->mPosition);

            switch (drawing->mType) {
                case DrawingType::Rect:
                {
                    auto               rect = std::static_pointer_cast<DrawingsRect>(drawing);
                    sf::RectangleShape shape(rect->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(rect->mFillColor);

                    if (rect->mIsOutlined == true) {
                        shape.setOutlineThickness(rect->mOutlineThickness);
                        shape.setOutlineColor(rect->mOutlineColor);
                    }

                    // TODO: Implement rounded rectangles when (mIsRounded == true)

                    window.draw(shape);

                    break;
                }

                case DrawingType::Text:
                {
                    auto     text_drawing = std::static_pointer_cast<DrawingsText>(drawing);
                    sf::Text text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);

                    text.setPosition(pos_drawing);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture:
                {
                    auto               tex = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::RectangleShape shape(tex->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(tex->mFillColor);
                    shape.setTexture(tex->mTexture.get());

                    if (tex->mTextureSmoothing == true) {
                        tex->mTexture->setSmooth(true);
                    }

                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Canvas() = default;

        void Update(const Controls& controls, sf::Vector2f pos_panel) override {
            (void)controls;
            (void)pos_panel;
        }

        void Render(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f                                              pos_global = pos_panel + mPosition;
            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingRects) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }
            for (const auto& [id, drawing] : mDrawingTexts) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }
            for (const auto& [id, drawing] : mDrawingTextures) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            std::sort(all_drawings.begin(), all_drawings.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, drawing] : all_drawings) {
                RenderDrawing(window, drawing, pos_global);
            }
        }
    };

    class Panel {
    private:
        std::string  mName      = "Unnamed";
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::vector<std::shared_ptr<Widget>> mWidgets;

    public:
        Panel() = default;
    };
} // namespace Orbis