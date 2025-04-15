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
            return mMouseState.mIsLPressed;
        }

        bool GetIsRMousePressed() const {
            return mMouseState.mIsRPressed;
        }

        bool GetIsWMousePressed() const {
            return mMouseState.mIsWPressed;
        }

        bool GetIsScrolling() const {
            return mMouseState.mIsScrolling;
        }

        void SetMousePosition(sf::Vector2i pos_mouse) {
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};

            mMouseState.mPosition = pos_mouse_f;
        }

        void SetIsLMousePressed(bool is_lmouse_pressed) {
            mMouseState.mIsLPressed = is_lmouse_pressed;
        }

        void SetIsRMousePressed(bool is_rmouse_pressed) {
            mMouseState.mIsRPressed = is_rmouse_pressed;
        }

        void SetIsWMousePressed(bool is_wmouse_pressed) {
            mMouseState.mIsWPressed = is_wmouse_pressed;
        }

        void SetIsScrolling(bool is_scrolling) {
            mMouseState.mIsScrolling = is_scrolling;
        }
    };
}