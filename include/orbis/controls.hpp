#pragma once

#include <SFML/System.hpp>

namespace Orbis {
    class Controls {
    private:
        sf::Vector2i mMousePosition = {0, 0};
        bool mIsLMousePressed = false;
        bool mIsRMousePressed = false;
        bool mIsWMousePressed = false;
        bool mIsScrolling = false;

    public:
        sf::Vector2i GetMousePosition() { return mMousePosition; }
        bool GetIsLMousePressed() { return mIsLMousePressed; }
        bool GetIsRMousePressed() { return mIsRMousePressed; }
        bool GetIsWMousePressed() { return mIsWMousePressed; }
        bool GetIsScrolling() { return mIsScrolling; }

        void SetMousePosition(sf::Vector2i mouse_pos) { mMousePosition = mouse_pos; }
        void SetIsLMousePressed(bool is_lmouse_pressed) { mIsLMousePressed = is_lmouse_pressed; }
        void SetIsRMousePressed(bool is_rmouse_pressed) { mIsRMousePressed = is_rmouse_pressed; }
        void SetIsWMousePressed(bool is_wmouse_pressed) { mIsWMousePressed = is_wmouse_pressed; }
        void SetIsScrolling(bool is_scrolling) { mIsScrolling = is_scrolling; }
    };
}