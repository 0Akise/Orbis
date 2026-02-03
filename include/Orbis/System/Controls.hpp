#pragma once

#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Orbis {
    struct Mouse {
        bool mIsLPressed  = false; // Left
        bool mIsRPressed  = false; // Right
        bool mIsWPressed  = false; // Wheel
        bool mIsE1Pressed = false; // Extra1
        bool mIsE2Pressed = false; // Extra2
        bool mIsScrolling = false;

        sf::Vector2f mPosition = {0, 0};

        std::vector<sf::Mouse::Button> mButtonsPressed;
        std::vector<sf::Mouse::Button> mButtonsReleased;

        void ClearFrameEvents() {
            mButtonsPressed.clear();
            mButtonsReleased.clear();
        }
    };

    struct Keyboard {
        bool mIsCPressed = false; // Control
        bool mIsSPressed = false; // Shift
        bool mIsAPressed = false; // Alt

        sf::String                     mTextEntered;
        std::vector<sf::Keyboard::Key> mKeysPressed;
        std::vector<sf::Keyboard::Key> mKeysReleased;

        bool IsKeyPressed(sf::Keyboard::Key key) const {
            return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
        }

        bool HasText() const {
            return mTextEntered.isEmpty() == false;
        }

        void ClearFrameEvents() {
            mTextEntered.clear();
            mKeysPressed.clear();
            mKeysReleased.clear();
        }
    };

    class Controls {
    public:
        Mouse    mMouse;
        Keyboard mKeyboard;
    };
} // namespace Orbis