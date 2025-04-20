#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

#include "Orbis/Derma/Derma.hpp"

namespace Orbis {
    enum class DButtonState {
        Normal,
        Hover,
        Pressed,
    };

    class DFrame : public Derma {
    public:
        DFrame(size_t id)
            : Derma(DType::DFrame, id) {
            SetOptions(DOptionFlag::None);
        }
    };

    class DButton : public Derma {
    private:
        DButtonState mState;
        sf::Color mColorNormal = sf::Color::White;
        sf::Color mColorHover = sf::Color::White;
        sf::Color mColorPressed = sf::Color::White;
        std::string mText;
        std::function<void()> mCallback;

    public:
        DButton(size_t id)
            : Derma(DType::DButton, id),
              mState(DButtonState::Normal),
              mText("") {
            SetOptions(DOptionFlag::Selectable);

            mEventSystem.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                if (event.mIsInBounds == true) {
                    mState = DButtonState::Pressed;
                }
            });

            mEventSystem.RegisterListener(DEventType::MouseLUp, [this](const DEvent& event) {
                if ((event.mIsInBounds == true) && (mState == DButtonState::Pressed)) {
                    if (mCallback != nullptr) {
                        mCallback();
                    }
                }

                if (event.mIsInBounds == true) {
                    mState = DButtonState::Hover;
                } else {
                    mState = DButtonState::Normal;
                }
            });

            mEventSystem.RegisterListener(DEventType::MouseEnter, [this](const DEvent& event) {
                if (event.mControls.mMouse.mLPress == false) {
                    mState = DButtonState::Hover;
                }
            });

            mEventSystem.RegisterListener(DEventType::MouseLeave, [this](const DEvent& event) {
                if (event.mControls.mMouse.mLPress == false) {
                    mState = DButtonState::Normal;
                }
            });
        }

        sf::Color GetStateColor() const {
            switch (mState) {
                case DButtonState::Hover:
                    return mColorHover;
                case DButtonState::Pressed:
                    return mColorPressed;
                default:
                    return mColorNormal;
            }
        }

        const std::string& GetText() const {
            return mText;
        }

        DButton& SetStateColor(DButtonState state, sf::Color color) {
            switch (state) {
                case DButtonState::Normal:
                    mColorNormal = color;
                    break;
                case DButtonState::Hover:
                    mColorHover = color;
                    break;
                case DButtonState::Pressed:
                    mColorPressed = color;
                    break;
            }
            return *this;
        }

        DButton& SetText(std::string text) {
            mText = std::move(text);
            return *this;
        }

        DButton& SetCallback(std::function<void()> callback) {
            mCallback = std::move(callback);
            return *this;
        }
    };
}