#pragma once

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class TextboxSingle : public Widget {
    private:
        std::function<void()> mCallback;

        sf::String mText        = "";
        sf::String mPlaceholder = "";
        size_t     mFontSize;

        size_t mCursorPos      = 0;
        size_t mSelectionStart = 0;
        size_t mSelectionEnd   = 0;

    public:
        TextboxSingle() = default;

        TextboxSingle& SetCallback(std::function<void()> callback) {
            mCallback = std::move(callback);

            return *this;
        }
    };
} // namespace Orbis