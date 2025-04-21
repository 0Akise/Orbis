#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

#include "Orbis/Derma/Derma.hpp"
#include "Orbis/Derma/DermaEnums.hpp"

namespace Orbis {
    class DFrame : public Derma<DFrame> {
    public:
        explicit DFrame(size_t id)
            : Derma<DFrame>(DType::DFrame, id) {}

        void Initialize() override {
            SetOptions(DOption::None);
        }
    };

    class DWindow : public Derma<DWindow> {
    private:
        std::string mTitle = "Unnamed";
        float mTitlebarHeight = 20;
        bool mIsMoving;
        sf::Vector2f mMoveOffset;

    public:
        explicit DWindow(size_t id)
            : Derma<DWindow>(DType::DWindow, id),
              mIsMoving(false) {}

        void Initialize() override {
            SetOptions(DOption::Movable);

            mEventSystem.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                if (IsInTitlebar(event.mControls.mMouse.mPosition)) {
                    mIsMoving = true;
                    mMoveOffset = event.mControls.mMouse.mPosition - GetPositionGlobal();
                }
            });

            mEventSystem.RegisterListener(DEventType::MouseLUp, [this](const DEvent&) {
                mIsMoving = false;
            });

            mEventSystem.RegisterListener(DEventType::MouseMove, [this](const DEvent& event) {
                if (mIsMoving == true) {
                    SetPosition(event.mControls.mMouse.mPosition - mMoveOffset);
                }
            });
        }

        bool IsInTitlebar(const sf::Vector2f& position) const {
            sf::Vector2f pos_global = GetPositionGlobal();

            return (pos_global.x <= position.x && position.x <= pos_global.x + GetSize().x &&
                    pos_global.y <= position.y && position.y <= pos_global.y + mTitlebarHeight);
        }

        const std::string& GetTitle() const {
            return mTitle;
        }

        float GetTitlebarHeight() const {
            return mTitlebarHeight;
        }
    };

    class DButton : public Derma<DButton> {
    private:
        DButtonState mState;
        sf::Color mColorNormal = sf::Color::White;
        sf::Color mColorHover = sf::Color::White;
        sf::Color mColorPressed = sf::Color::White;
        std::string mText;
        std::function<void()> mCallback;

    public:
        explicit DButton(size_t id)
            : Derma<DButton>(DType::DButton, id),
              mState(DButtonState::Normal),
              mText("") {}

        void Initialize() override {
            SetOptions(DOption::Selectable);

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

    template <typename T>
    struct DTypeMap {
        static constexpr DType value = static_cast<DType>(-1);
    };

    template <>
    struct DTypeMap<DFrame> {
        static constexpr DType value = DType::DFrame;
    };

    template <>
    struct DTypeMap<DWindow> {
        static constexpr DType value = DType::DWindow;
    };

    template <>
    struct DTypeMap<DButton> {
        static constexpr DType value = DType::DButton;
    };
}