#pragma once

#include <functional>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class Button : public Widget {
    private:
        std::function<void()> mOnButtonPressed;

        ButtonState mState = ButtonState::Normal;

        sf::Color mColorNormal  = sf::Color(200, 200, 200, 255);
        sf::Color mColorHover   = sf::Color(220, 220, 220, 255);
        sf::Color mColorPressed = sf::Color(150, 150, 150, 255);

        bool mWasPressed = false;

        void RenderDrawing(sf::RenderWindow&                window,
                           const std::shared_ptr<Drawings>& drawing,
                           sf::Vector2f                     pos_widget) {
            sf::Vector2f pos_drawing = pos_widget + drawing->mPosition;

            switch (drawing->mType) {
                case DrawingType::Line: {
                    auto line = std::static_pointer_cast<DrawingsLine>(drawing);

                    if (line->mPoints.size() < 2) {
                        break;
                    }

                    if (line->mThickness <= 1.0f) {
                        sf::VertexArray curve(sf::PrimitiveType::LineStrip);

                        for (const auto& point : line->mPoints) {
                            curve.append(sf::Vertex(pos_drawing + point, line->mFillColor));
                        }

                        window.draw(curve);
                    }
                    else {
                        sf::VertexArray line_thick     = sf::VertexArray(sf::PrimitiveType::Triangles);
                        float           thickness_half = line->mThickness / 2.0f;

                        for (size_t i = 0; i < line->mPoints.size() - 1; ++i) {
                            sf::Vector2f p1 = pos_drawing + line->mPoints[i];
                            sf::Vector2f p2 = pos_drawing + line->mPoints[i + 1];

                            sf::Vector2f direction = p2 - p1;
                            float        length    = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                            if (length < 0.001f) {
                                continue;
                            }

                            sf::Vector2f perpendicular = {-direction.y / length, direction.x / length};
                            sf::Vector2f offset        = perpendicular * thickness_half;

                            sf::Vector2f v1 = p1 + offset;
                            sf::Vector2f v2 = p1 - offset;
                            sf::Vector2f v3 = p2 - offset;
                            sf::Vector2f v4 = p2 + offset;

                            line_thick.append(sf::Vertex(v1, line->mFillColor));
                            line_thick.append(sf::Vertex(v2, line->mFillColor));
                            line_thick.append(sf::Vertex(v3, line->mFillColor));

                            line_thick.append(sf::Vertex(v1, line->mFillColor));
                            line_thick.append(sf::Vertex(v3, line->mFillColor));
                            line_thick.append(sf::Vertex(v4, line->mFillColor));
                        }

                        window.draw(line_thick);
                    }

                    break;
                }

                case DrawingType::Rect: {
                    // TODO : Implement Outlines

                    auto      rect        = std::static_pointer_cast<DrawingsRect>(drawing);
                    sf::Color state_color = GetStateColor();

                    if (rect->mIsRounded == true) {
                        sf::ConvexShape shape = sf::RectRounded(rect->mSize, rect->mRoundingRadius);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(state_color);

                        if (rect->mIsOutlined == true) {
                            shape.setOutlineThickness(rect->mOutlineThickness);
                            shape.setOutlineColor(rect->mOutlineColor);
                        }

                        window.draw(shape);
                    }
                    else {
                        sf::RectangleShape shape = sf::RectangleShape(rect->mSize);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(state_color);

                        if (rect->mIsOutlined == true) {
                            shape.setOutlineThickness(rect->mOutlineThickness);
                            shape.setOutlineColor(rect->mOutlineColor);
                        }

                        window.draw(shape);
                    }

                    break;
                }

                case DrawingType::Text: {
                    auto          text_drawing = std::static_pointer_cast<DrawingsText>(drawing);
                    sf::Text      text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);
                    sf::FloatRect bounds = text.getLocalBounds();
                    sf::Vector2f  offset = {0, 0};

                    float offset_x = 0.0f;
                    float offset_y = 0.0f;

                    if (text_drawing->mAlign == TextAlign::CenterTop ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -bounds.size.x / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop ||
                             text_drawing->mAlign == TextAlign::RightCenter ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -bounds.size.x;
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -static_cast<float>(text_drawing->mFontSize) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom ||
                             text_drawing->mAlign == TextAlign::CenterBottom ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -static_cast<float>(text_drawing->mFontSize);
                    }

                    offset = {offset_x, offset_y};

                    text.setPosition(pos_drawing + offset);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture: {
                    auto               texture = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::RectangleShape shape(texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setTexture(texture->mTexture.get());

                    if (texture->mTextureSmoothing == true) {
                        texture->mTexture->setSmooth(true);
                    }

                    sf::Color blended_color = texture->mFillColor;
                    sf::Color state_color   = GetStateColor();

                    blended_color.r = (blended_color.r * state_color.r) / 255;
                    blended_color.g = (blended_color.g * state_color.g) / 255;
                    blended_color.b = (blended_color.b * state_color.b) / 255;
                    blended_color.a = (blended_color.a * state_color.a) / 255;

                    shape.setFillColor(blended_color);
                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Button() = default;

        ButtonState GetState() const {
            return mState;
        }

        sf::Color GetStateColor() const {
            switch (mState) {
                case ButtonState::Hover:
                    return mColorHover;
                case ButtonState::Pressed:
                    return mColorPressed;
                default:
                    return mColorNormal;
            }
        }

        Button& SetOnButtonPressed(std::function<void()> callback) {
            mOnButtonPressed = std::move(callback);

            return *this;
        }

        Button& SetStateColor(ButtonState state, sf::Color color) {
            switch (state) {
                case ButtonState::Normal: {
                    mColorNormal = color;

                    break;
                }

                case ButtonState::Hover: {
                    mColorHover = color;

                    break;
                }

                case ButtonState::Pressed: {
                    mColorPressed = color;

                    break;
                }
            }

            return *this;
        }

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<Button>();

            cloned->mSize      = mSize;
            cloned->mPosition  = mPosition;
            cloned->mZLevel    = mZLevel;
            cloned->mIsVisible = mIsVisible;

            cloned->mOnButtonPressed = mOnButtonPressed;
            cloned->mColorNormal     = mColorNormal;
            cloned->mColorHover      = mColorHover;
            cloned->mColorPressed    = mColorPressed;

            cloned->mState      = ButtonState::Normal;
            cloned->mWasPressed = false;

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing       = std::make_shared<DrawingsRect>(*drawing);
                cloned->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing       = std::make_shared<DrawingsText>(*drawing);
                cloned->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing          = std::make_shared<DrawingsTexture>(*drawing);
                cloned->mDrawingsTexture[id] = cloned_drawing;
            }

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f  pos_global = pos_panel + mPosition;
            sf::FloatRect bounds(pos_global, mSize);

            bool is_hovered = bounds.contains(controls.mMouse.mPosition);

            if (is_hovered == true) {
                if (controls.mMouse.mIsLPressed == true) {
                    mState      = ButtonState::Pressed;
                    mWasPressed = true;
                }
                else {
                    if (mWasPressed == true) {
                        if (mOnButtonPressed) {
                            mOnButtonPressed();
                        }

                        mWasPressed = false;
                    }

                    mState = ButtonState::Hover;
                }
            }
            else {
                mState = ButtonState::Normal;

                if (controls.mMouse.mIsLPressed == false) {
                    mWasPressed = false;
                }
            }
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f pos_global = pos_panel + mPosition;

            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingsLine) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsRect) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
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
} // namespace Orbis