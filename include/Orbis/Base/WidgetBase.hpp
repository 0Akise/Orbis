#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Base/Drawing.hpp"
#include "Orbis/Base/WidgetInterface.hpp"

namespace Orbis {
    template <typename Derived>
    class WidgetBase : public WidgetInterface {
    private:
        sf::Vector2f mSize = {0, 0};
        sf::Vector2f mPosition = {0, 0};
        sf::Vector2f mPositionOffset = {0, 0};
        size_t mZLevel = 0;

        std::multimap<size_t, std::shared_ptr<Drawing>> mDrawings;

        bool mIsVisible = true;
        bool mIsInBounds = false;

    protected:
        Derived& self() {
            return *static_cast<Derived*>(this);
        }

        const Derived& self() const {
            return *static_cast<const Derived*>(this);
        }

    public:
        explicit WidgetBase() {}

        template <typename T = Derived>
        std::shared_ptr<T> GetSharedFromThis() {
            return std::dynamic_pointer_cast<T>(WidgetBase::shared_from_this());
        }

        void SetPositionOffset(sf::Vector2f offset) override {
            mPositionOffset = offset;
        }

        sf::Vector2f GetSize() const {
            return mSize;
        }

        sf::Vector2f GetPosition() const {
            return mPosition;
        }

        sf::Vector2f GetPositionGlobal() const override {
            return mPosition + mPositionOffset;
        }

        size_t GetZLevel() const {
            return mZLevel;
        }

        bool GetVisiblityStatus() const {
            return mIsVisible;
        }

        Derived& SetSize(sf::Vector2f size) {
            mSize = size;

            return self();
        }

        Derived& SetPosition(sf::Vector2f position) {
            mPosition = position;

            return self();
        }

        Derived& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;

            return self();
        }

        Derived& SetVisible(bool is_visible) {
            mIsVisible = is_visible;

            return self();
        }

        void Update(const Controls&) override {
            if (mIsVisible == false)
                return;
        }

        void Render(sf::RenderWindow& window) override {
            if (mIsVisible == false)
                return;

            sf::Vector2f pos_global = GetPositionGlobal();

            for (const auto& [_, drawing] : mDrawings) {
                ProcessDrawings(window, drawing, pos_global);
            }
        }

        void ProcessDrawings(sf::RenderWindow& window, const std::shared_ptr<Drawing>& drawing, const sf::Vector2f& pos_global) {
            sf::Vector2f pos_drawing = pos_global + drawing->mPosition;

            switch (drawing->mType) {
                case DrawingType::Rect: {
                    auto drawing_rect = std::static_pointer_cast<DrawingRect>(drawing);

                    if (drawing_rect->mIsRounded == true) {
                        break;
                    } else {
                        sf::RectangleShape shape(drawing_rect->mSize);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(drawing_rect->mFillColor);

                        if (drawing_rect->mIsOutlined == true) {
                            shape.setOutlineThickness(drawing_rect->mOutlineThickness);
                            shape.setOutlineColor(drawing_rect->mOutlineColor);
                        }

                        window.draw(shape);

                        break;
                    }
                }

                case DrawingType::Text: {
                    auto drawing_text = std::static_pointer_cast<DrawingText>(drawing);
                    sf::Text text(drawing_text->mFont, drawing_text->mText, drawing_text->mFontSize);

                    text.setPosition(pos_drawing);
                    text.setFillColor(drawing_text->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture: {
                    auto drawing_texture = std::static_pointer_cast<DrawingTexture>(drawing);

                    sf::RectangleShape shape(drawing_texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(drawing_texture->mFillColor);

                    if (drawing_texture->mTextureSmoothing == true) {
                        drawing_texture->mTexture.setSmooth(true);
                    }

                    shape.setTexture(&drawing_texture->mTexture);
                    window.draw(shape);

                    break;
                }
            }
        }

        Derived& DrawRect(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f) {
            auto drawing = std::make_shared<DrawingRect>();

            drawing->mType = DrawingType::Rect;
            drawing->mID = "";
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mIsOutlined = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor = outline_color;
            drawing->mIsRounded = is_rounded;
            drawing->mRoundingRadius = rounding_radius;
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawRectDynamic(
            const std::string& id,
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f) {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingRect>();

            drawing->mType = DrawingType::Rect;
            drawing->mID = std::move(id);
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mIsOutlined = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor = outline_color;
            drawing->mIsRounded = is_rounded;
            drawing->mRoundingRadius = rounding_radius;
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawText(
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            const std::string& text = "") {
            auto drawing = std::make_shared<DrawingText>();

            drawing->mType = DrawingType::Text;
            drawing->mID = "";
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTextDynamic(
            const std::string& id,
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            const std::string& text = "") {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingText>();

            drawing->mType = DrawingType::Text;
            drawing->mID = std::move(id);
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTexture(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            sf::Texture texture,
            bool smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingTexture>();

            drawing->mType = DrawingType::Texture;
            drawing->mID = "";
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mTexture = texture;
            drawing->mTextureSmoothing = smoothing_enabled;
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTextureDynamic(
            const std::string& id,
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            sf::Texture texture,
            bool smoothing_enabled = true) {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingTexture>();

            drawing->mType = DrawingType::Texture;
            drawing->mID = std::move(id);
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mTexture = texture;
            drawing->mTextureSmoothing = smoothing_enabled;
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& ClearDrawing(const std::string& id) {
            if (id.empty() == true) {
                return self();
            }

            std::vector<std::multimap<size_t, std::shared_ptr<Drawing>>::iterator> to_erase;

            for (auto iter = mDrawings.begin(); iter != mDrawings.end(); ++iter) {
                if (iter->second->mID == id) {
                    to_erase.push_back(iter);
                }
            }

            for (auto& iter : to_erase) {
                mDrawings.erase(iter);
            }

            return self();
        }

        Derived& ClearDrawingsAll() {
            mDrawings.clear();

            return self();
        }
    };
}