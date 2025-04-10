#pragma once

#include <SFML/System.hpp>

namespace Orbis {
    class Controls {
    private:
        sf::Vector2i mMousePosition;
        bool mIsLMousePressed;
        bool mIsRMousePressed;
        bool mIsWMousePressed;
        bool mIsScrolling;

    public:
        Controls()
            : mMousePosition({0, 0}),
              mIsLMousePressed(false),
              mIsRMousePressed(false),
              mIsWMousePressed(false),
              mIsScrolling(false) {}

        static Controls Create() {
            return Controls();
        }

        sf::Vector2i GetMousePosition() const {
            return mMousePosition;
        }
        
        bool GetIsLMousePressed() const {
            return mIsLMousePressed;
        }
        
        bool GetIsRMousePressed() const {
            return mIsRMousePressed;
        }
        
        bool GetIsWMousePressed() const {
            return mIsWMousePressed;
        }
        
        bool GetIsScrolling() const {
            return mIsScrolling;
        }

        void SetMousePosition(sf::Vector2i mouse_pos) { 
            mMousePosition = mouse_pos;
        }
        
        void SetIsLMousePressed(bool is_lmouse_pressed) {
            mIsLMousePressed = is_lmouse_pressed;
        }
        
        void SetIsRMousePressed(bool is_rmouse_pressed) {
            mIsRMousePressed = is_rmouse_pressed;
        }
        
        void SetIsWMousePressed(bool is_wmouse_pressed) {
            mIsWMousePressed = is_wmouse_pressed;
        }
        
        void SetIsScrolling(bool is_scrolling) {
            mIsScrolling = is_scrolling;
        }
    };
}