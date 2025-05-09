#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

#include "Orbis/Derma/Derma.hpp"
#include "Orbis/Derma/DermaCommons.hpp"

namespace Orbis {
    class DFrame : public Derma<DFrame> {
    public:
        explicit DFrame(size_t id)
            : Derma<DFrame>(id) {}

        void Initialize() {
            this->SetOptions(DOption::None);
        }
    };

    class DWindow : public Derma<DWindow> {
    private:
        std::string mTitle = "Unnamed";
        float mTitlebarHeight = 20;
        [[maybe_unused]] bool mIsMoving;
        sf::Vector2f mMoveOffset;

    public:
        explicit DWindow(size_t id)
            : Derma<DWindow>(id),
              mIsMoving(false) {}

        void Initialize() {
            this->SetOptions(DOption::Selectable);
        }

        bool IsInTitlebar(const sf::Vector2f& position) const {
            sf::Vector2f pos_global = this->GetPositionGlobal();

            return (pos_global.x <= position.x && position.x <= pos_global.x + this->GetSize().x &&
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
            : Derma<DButton>(id),
              mState(DButtonState::Normal),
              mText("") {}

        void Initialize() {
            this->SetOptions(DOption::Selectable);

            this->mEventSystem.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                if (event.mIsInBounds == true) {
                    mState = DButtonState::Pressed;
                }
            });

            this->mEventSystem.RegisterListener(DEventType::MouseLUp, [this](const DEvent& event) {
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

            this->mEventSystem.RegisterListener(DEventType::MouseEnter, [this](const DEvent& event) {
                if (event.mControls.mMouse.mLPress == false) {
                    mState = DButtonState::Hover;
                }
            });

            this->mEventSystem.RegisterListener(DEventType::MouseLeave, [this](const DEvent& event) {
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

            return self();
        }

        DButton& SetText(const std::string& text) {
            mText = std::move(text);

            return self();
        }

        DButton& SetCallback(std::function<void()> callback) {
            mCallback = std::move(callback);

            return self();
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