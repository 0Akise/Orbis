#pragma once

#include <SFML/System.hpp>

#include "Orbis/Data.hpp"

namespace Orbis {
    class Controls {
    private:
        MouseState mMouseState;

    public:
        Controls() {}

        static Controls Create() {
            return Controls();
        }

        sf::Vector2f GetMousePosition() const {
            return mMouseState.mPosition;
        }

        bool GetIsLMousePressed() const {
            return mMouseState.mLPress;
        }

        bool GetIsRMousePressed() const {
            return mMouseState.mRPress;
        }

        bool GetIsWMousePressed() const {
            return mMouseState.mWPress;
        }

        bool GetIsScrolling() const {
            return mMouseState.mScroll;
        }

        void SetMousePosition(sf::Vector2i pos_mouse) {
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};

            mMouseState.mPosition = pos_mouse_f;
        }

        void SetIsLMousePressed(bool is_lmouse_pressed) {
            mMouseState.mLPress = is_lmouse_pressed;
        }

        void SetIsRMousePressed(bool is_rmouse_pressed) {
            mMouseState.mRPress = is_rmouse_pressed;
        }

        void SetIsWMousePressed(bool is_wmouse_pressed) {
            mMouseState.mWPress = is_wmouse_pressed;
        }

        void SetIsScrolling(bool is_scrolling) {
            mMouseState.mScroll = is_scrolling;
        }
    };
}