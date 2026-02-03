#pragma once

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class Slider : public Widget {
    private:
        std::function<void(float)> mCallback;

        float mValueMin = 0.0f;
        float mValueMax = 1.0f;
        float mValue    = 0.5f;
        float mStepSize = 0.0f;

        bool mIsHorizontal = true;

        SliderState  mState      = SliderState::Normal;
        bool         mIsDragging = false;
        sf::Vector2f mDragOffset = {0, 0};

        sf::Vector2f mHandleSize          = {16.0f, 16.0f};
        float        mHandleRadius        = 8.0f;
        bool         mHandleRounded       = true;
        sf::Color    mHandleColorNormal   = sf::Color(200, 200, 200, 255);
        sf::Color    mHandleColorHover    = sf::Color(220, 220, 220, 255);
        sf::Color    mHandleColorDragging = sf::Color(150, 150, 150, 255);

        sf::Vector2f mTrackSize   = {200.0f, 4.0f};
        sf::Vector2f mTrackOffset = {0.0f, 0.0f};
        sf::Color    mTrackColor  = sf::Color(80, 80, 80, 255);

        bool      mShowFill  = true;
        sf::Color mFillColor = sf::Color(100, 150, 255, 255);

        sf::Vector2f GetHandlePosition() const {
            float normalized = 0.0f;

            if (mValueMax != mValueMin) {
                normalized = (mValue - mValueMin) / (mValueMax - mValueMin);
            }

            if (mIsHorizontal == true) {
                float x = mTrackSize.x * normalized;
                float y = 0.0f;

                return {x, y};
            }
            else {
                float x = 0.0f;
                float y = mTrackSize.y * (1.0f - normalized);

                return {x, y};
            }
        }

        float CalculateValueFromPos(sf::Vector2f mouse_pos, sf::Vector2f widget_pos) const {
            sf::Vector2f track_pos  = widget_pos + mTrackOffset;
            float        normalized = 0.0f;

            if (mIsHorizontal == true) {
                float relative = mouse_pos.x - track_pos.x;

                normalized = relative / mTrackSize.x;
            }
            else {
                float relative = mouse_pos.y - track_pos.y;

                normalized = 1.0f - (relative / mTrackSize.y);
            }

            normalized = std::max(0.0f, std::min(1.0f, normalized));

            float value = mValueMin + normalized * (mValueMax - mValueMin);

            if (mStepSize > 0.0f) {
                value = std::round(value / mStepSize) * mStepSize;
            }

            return std::max(mValueMin, std::min(mValueMax, value));
        }

        sf::Color GetHandleColor() const {
            switch (mState) {
                case SliderState::Hover:
                    return mHandleColorHover;
                case SliderState::Dragging:
                    return mHandleColorDragging;
                default:
                    return mHandleColorNormal;
            }
        }

        void RenderComponents(sf::RenderWindow& window, sf::Vector2f pos_widget) {
            sf::RectangleShape track(mTrackSize);
            track.setPosition(pos_widget + mTrackOffset);
            track.setFillColor(mTrackColor);
            window.draw(track);

            if (mShowFill == true) {
                sf::Vector2f fill_size = mTrackSize;

                float normalized = 0.0f;

                if (mValueMax != mValueMin) {
                    normalized = (mValue - mValueMin) / (mValueMax - mValueMin);
                }

                if (mIsHorizontal == true) {
                    fill_size.x *= normalized;
                }
                else {
                    fill_size.y *= normalized;
                }

                sf::RectangleShape fill(fill_size);
                sf::Vector2f       fill_offset = mTrackOffset;

                if (mIsHorizontal == false) {
                    fill_offset.y += mTrackSize.y - fill_size.y;
                }

                fill.setPosition(pos_widget + fill_offset);
                fill.setFillColor(mFillColor);
                window.draw(fill);
            }

            sf::Vector2f handle_pos = pos_widget + mTrackOffset + GetHandlePosition();

            if (mIsHorizontal == true) {
                handle_pos.x -= mHandleSize.x / 2.0f;
                handle_pos.y -= (mHandleSize.y - mTrackSize.y) / 2.0f;
            }
            else {
                handle_pos.x -= (mHandleSize.x - mTrackSize.x) / 2.0f;
                handle_pos.y -= mHandleSize.y / 2.0f;
            }

            if (mHandleRounded == true) {
                sf::ConvexShape handle = sf::RectRounded(mHandleSize, mHandleRadius);

                handle.setPosition(handle_pos);
                handle.setFillColor(GetHandleColor());
                window.draw(handle);
            }
            else {
                sf::RectangleShape handle(mHandleSize);

                handle.setPosition(handle_pos);
                handle.setFillColor(GetHandleColor());
                window.draw(handle);
            }
        }

        sf::FloatRect GetHandleBounds(sf::Vector2f widget_pos) const {
            sf::Vector2f handle_pos = widget_pos + mTrackOffset + GetHandlePosition();

            if (mIsHorizontal == true) {
                handle_pos.y -= (mHandleSize.y - mTrackSize.y) / 2.0f;
            }
            else {
                handle_pos.x -= (mHandleSize.x - mTrackSize.x) / 2.0f;
            }

            return sf::FloatRect(handle_pos, mHandleSize);
        }

        void RenderCustomDrawings(sf::RenderWindow&                window,
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
                    auto rect = std::static_pointer_cast<DrawingsRect>(drawing);

                    if (rect->mIsRounded == true) {
                        sf::ConvexShape shape = sf::RectRounded(rect->mSize, rect->mRoundingRadius);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(rect->mFillColor);

                        if (rect->mIsOutlined == true) {
                            shape.setOutlineThickness(rect->mOutlineThickness);
                            shape.setOutlineColor(rect->mOutlineColor);
                        }

                        window.draw(shape);
                    }
                    else {
                        sf::RectangleShape shape(rect->mSize);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(rect->mFillColor);

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

                    if (text_drawing->mAlign == TextAlign::CenterTop || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -(bounds.size.x) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop || text_drawing->mAlign == TextAlign::RightCenter || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -(bounds.size.x);
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize)) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom || text_drawing->mAlign == TextAlign::CenterBottom || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize));
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
                    shape.setFillColor(texture->mFillColor);
                    shape.setTexture(texture->mTexture.get());

                    if (texture->mTextureSmoothing == true) {
                        texture->mTexture->setSmooth(true);
                    }

                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Slider() = default;

        float GetValue() const {
            return mValue;
        }

        float GetMin() const {
            return mValueMin;
        }

        float GetMax() const {
            return mValueMax;
        }

        bool IsHorizontal() const {
            return mIsHorizontal;
        }

        Slider& SetRange(float min, float max) {
            mValueMin = min;
            mValueMax = max;
            mValue    = std::max(mValueMin, std::min(mValueMax, mValue));

            return *this;
        }

        Slider& SetValue(float value) {
            mValue = std::max(mValueMin, std::min(mValueMax, value));

            return *this;
        }

        Slider& SetStepSize(float step) {
            mStepSize = step;

            return *this;
        }

        Slider& SetOrientation(bool horizontal) {
            mIsHorizontal = horizontal;

            return *this;
        }

        Slider& SetCallback(std::function<void(float)> callback) {
            mCallback = std::move(callback);

            return *this;
        }

        Slider& SetTrackSize(sf::Vector2f size) {
            mTrackSize = size;

            return *this;
        }

        Slider& SetTrackOffset(sf::Vector2f offset) {
            mTrackOffset = offset;

            return *this;
        }

        Slider& SetTrackColor(sf::Color color) {
            mTrackColor = color;

            return *this;
        }

        Slider& SetShowFill(bool show) {
            mShowFill = show;

            return *this;
        }

        Slider& SetFillColor(sf::Color color) {
            mFillColor = color;

            return *this;
        }

        Slider& SetHandleSize(sf::Vector2f size) {
            mHandleSize = size;

            return *this;
        }

        Slider& SetHandleRadius(float radius) {
            mHandleRadius = radius;

            return *this;
        }

        Slider& SetHandleRounded(bool rounded) {
            mHandleRounded = rounded;

            return *this;
        }

        Slider& SetHandleColor(SliderState state, sf::Color color) {
            switch (state) {
                case SliderState::Normal:
                    mHandleColorNormal = color;
                    break;
                case SliderState::Hover:
                    mHandleColorHover = color;
                    break;
                case SliderState::Dragging:
                    mHandleColorDragging = color;
                    break;
            }
            return *this;
        }

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<Slider>();

            cloned->mSize      = mSize;
            cloned->mPosition  = mPosition;
            cloned->mZLevel    = mZLevel;
            cloned->mIsVisible = mIsVisible;

            cloned->mValueMin     = mValueMin;
            cloned->mValueMax     = mValueMax;
            cloned->mValue        = mValue;
            cloned->mStepSize     = mStepSize;
            cloned->mIsHorizontal = mIsHorizontal;
            cloned->mCallback     = mCallback;

            cloned->mHandleSize          = mHandleSize;
            cloned->mHandleRadius        = mHandleRadius;
            cloned->mHandleRounded       = mHandleRounded;
            cloned->mHandleColorNormal   = mHandleColorNormal;
            cloned->mHandleColorHover    = mHandleColorHover;
            cloned->mHandleColorDragging = mHandleColorDragging;

            cloned->mTrackSize   = mTrackSize;
            cloned->mTrackOffset = mTrackOffset;
            cloned->mTrackColor  = mTrackColor;
            cloned->mShowFill    = mShowFill;
            cloned->mFillColor   = mFillColor;

            cloned->mState      = SliderState::Normal;
            cloned->mIsDragging = false;
            cloned->mDragOffset = {0.0f, 0.0f};

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing = std::make_shared<DrawingsRect>(*drawing);

                cloned->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing = std::make_shared<DrawingsText>(*drawing);

                cloned->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing = std::make_shared<DrawingsTexture>(*drawing);

                cloned->mDrawingsTexture[id] = cloned_drawing;
            }

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f  pos_global    = pos_panel + mPosition;
            sf::FloatRect handle_bounds = GetHandleBounds(pos_global);
            sf::FloatRect track_bounds(pos_global + mTrackOffset, mTrackSize);

            bool is_handle_hovered = handle_bounds.contains(controls.mMouse.mPosition);
            bool is_track_clicked  = track_bounds.contains(controls.mMouse.mPosition);

            if (mIsDragging == true) {
                if (controls.mMouse.mIsLPressed == true) {
                    float new_value = CalculateValueFromPos(controls.mMouse.mPosition, pos_global);

                    if (new_value != mValue) {
                        mValue = new_value;

                        if (mCallback) {
                            mCallback(mValue);
                        }
                    }

                    mState = SliderState::Dragging;
                }
                else {
                    mIsDragging = false;
                    mState      = is_handle_hovered ? SliderState::Hover : SliderState::Normal;
                }
            }
            else {
                if (controls.mMouse.mIsLPressed == true) {
                    if (is_handle_hovered == true) {
                        mIsDragging = true;
                        mDragOffset = controls.mMouse.mPosition - (pos_global + mTrackOffset + GetHandlePosition());
                        mState      = SliderState::Dragging;
                    }
                    else if (is_track_clicked == true) {
                        float new_value = CalculateValueFromPos(controls.mMouse.mPosition, pos_global);

                        if (new_value != mValue) {
                            mValue = new_value;

                            if (mCallback) {
                                mCallback(mValue);
                            }
                        }

                        mIsDragging = true;
                        mState      = SliderState::Dragging;
                    }
                }
                else {
                    mState = is_handle_hovered ? SliderState::Hover : SliderState::Normal;
                }
            }
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f pos_global = pos_panel + mPosition;

            RenderComponents(window, pos_global);

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
                RenderCustomDrawings(window, drawing, pos_global);
            }
        }
    };
} // namespace Orbis