#pragma once

#include <SFML/System.hpp>

namespace Orbis {
    struct Mouse {
        sf::Vector2f mPosition = {0, 0};
        bool         mLPress   = false;
        bool         mRPress   = false;
        bool         mWPress   = false;
        bool         mScroll   = false;
    };

    struct Keyboard {};

    class Controls {
    public:
        Mouse    mMouse;
        Keyboard mKeyboard;
    };
} // namespace Orbis