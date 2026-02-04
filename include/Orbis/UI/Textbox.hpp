#pragma once

#include <chrono>
#include <functional>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class TextboxSingle : public Widget {
    private:
        std::function<void(const sf::String&)> mOnTextChanged;
        std::function<void()>                  mOnEnterPressed;

        sf::String  mText        = "";
        sf::String  mPlaceholder = "";
        std::string mEditableID  = "";

        size_t mCursorPos      = 0;
        size_t mSelectionStart = 0;
        size_t mSelectionEnd   = 0;

        TextboxState mState       = TextboxState::Normal;
        CursorStyle  mCursorStyle = CursorStyle::Line;

        bool                                  mIsCursorVisible = false;
        std::chrono::steady_clock::time_point mCursorLastBlink;

    public:
        TextboxSingle() = default;

        const sf::String& GetText() const {
            return mText;
        }

        TextboxSingle& SetOnTextChange(std::function<void(const sf::String&)> callback) {
            mOnTextChanged = std::move(callback);

            return *this;
        }

        TextboxSingle& SetOnEnterPressed(std::function<void()> callback) {
            mOnEnterPressed = std::move(callback);

            return *this;
        }

        TextboxSingle& SetPlaceholder(const sf::String& placeholder) {
            mPlaceholder = placeholder;

            return *this;
        }

        TextboxSingle& SetEdiableText(const std::string& text_id) {
            mEditableID = text_id;

            return *this;
        }
    };
} // namespace Orbis