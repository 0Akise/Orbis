#pragma once

#include <functional>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/Widgets/Widget.hpp"

namespace Orbis {
    class Button : public Widget {
    private:
        std::function<void()> mOnButtonPressed;

        ButtonState mState = ButtonState::Normal;

        sf::Color mColorNormal  = sf::Color(200, 200, 200, 255);
        sf::Color mColorHover   = sf::Color(220, 220, 220, 255);
        sf::Color mColorPressed = sf::Color(150, 150, 150, 255);

        bool mWasPressed = false;

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

            cloned->mSize            = mSize;
            cloned->mPosition        = mPosition;
            cloned->mZLevel          = mZLevel;
            cloned->mIsVisible       = mIsVisible;
            cloned->mOnButtonPressed = mOnButtonPressed;
            cloned->mColorNormal     = mColorNormal;
            cloned->mColorHover      = mColorHover;
            cloned->mColorPressed    = mColorPressed;
            cloned->mState           = ButtonState::Normal;
            cloned->mWasPressed      = false;

            CloneDrawingsTo(cloned.get());

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            UpdateAnimation();

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

            auto color_mod = [this](DrawingType type, const sf::Color& original) -> sf::Color {
                if (type == DrawingType::Rect) {
                    return GetStateColor();
                }
                else if (type == DrawingType::Texture) {
                    sf::Color state   = GetStateColor();
                    sf::Color blended = original;

                    blended.r = (blended.r * state.r) / 255;
                    blended.g = (blended.g * state.g) / 255;
                    blended.b = (blended.b * state.b) / 255;
                    blended.a = (blended.a * state.a) / 255;

                    return blended;
                }

                return original;
            };

            RenderAllDrawings(window, pos_global, color_mod);
        }
    };
} // namespace Orbis