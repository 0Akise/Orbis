#pragma once;

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class TextboxSingle : public Widget {
    private:
        std::function<void(float)> mCallback;

        sf::String mText        = "";
        sf::String mPlaceholder = "";
        size_t     mFontSize;

        size_t mCursorPos      = 0;
        size_t mSelectionStart = 0;
        size_t mSelectionEnd   = 0;
    };
} // namespace Orbis