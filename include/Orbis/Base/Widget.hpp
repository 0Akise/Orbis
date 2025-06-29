#pragma once

#include <functional>

#include <SFML/Graphics.hpp>

#include "Orbis/Base/WidgetBase.hpp"

namespace Orbis {
    enum class WidgetType {
        Panel,
        Button,
    };

    enum class ButtonState {
        Normal,
        Hover,
        Pressed,
    };

    class Panel : public WidgetBase<Panel> {
    public:
        explicit Panel() {}
    };

    class Button : public WidgetBase<Button> {
    private:
        ButtonState mState;
        std::string mText;
        sf::Color mColorNormal = sf::Color::White;
        sf::Color mColorHover = sf::Color::White;
        sf::Color mColorPressed = sf::Color::White;
        std::function<void()> mCallback;

    public:
        explicit Button()
            : WidgetBase<Button>(),
              mState(ButtonState::Normal),
              mText("") {}

        const std::string& GetText() const {
            return mText;
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

        Button& SetStateColor(ButtonState state, sf::Color color) {
            switch (state) {
                case ButtonState::Normal:
                    mColorNormal = color;
                    break;
                case ButtonState::Hover:
                    mColorHover = color;
                    break;
                case ButtonState::Pressed:
                    mColorPressed = color;
                    break;
            }

            return self();
        }

        Button& SetText(const std::string& text) {
            mText = std::move(text);

            return self();
        }

        Button& SetCallback(std::function<void()> callback) {
            mCallback = std::move(callback);

            return self();
        }
    };

    template <typename T>
    struct WidgetTypeMap {
        static constexpr WidgetType value = static_cast<WidgetType>(-1);
    };

    template <>
    struct WidgetTypeMap<Panel> {
        static constexpr WidgetType value = WidgetType::Panel;
    };

    template <>
    struct WidgetTypeMap<Button> {
        static constexpr WidgetType value = WidgetType::Button;
    };
}