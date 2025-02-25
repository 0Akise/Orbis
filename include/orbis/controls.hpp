#pragma once

#include <SFML/System.hpp>

namespace Orbis {
    class Controls {
    private:
        struct M {
            sf::Vector2i _MousePosition;
            bool _IsLMousePressed;
            bool _IsRMousePressed;
            bool _IsWMousePressed;
            bool _IsScrolling;
        } m;

        explicit Controls(M m) : m(std::move(m)) {}

    public:
        static Controls Create() {
            return Controls(M{
                ._MousePosition = {0, 0},
                ._IsLMousePressed = false,
                ._IsRMousePressed = false,
                ._IsWMousePressed = false,
                ._IsScrolling = false});
        }

        sf::Vector2i GetMousePosition() { return m._MousePosition; }
        bool GetIsLMousePressed() { return m._IsLMousePressed; }
        bool GetIsRMousePressed() { return m._IsRMousePressed; }
        bool GetIsWMousePressed() { return m._IsWMousePressed; }
        bool GetIsScrolling() { return m._IsScrolling; }

        void SetMousePosition(sf::Vector2i mouse_pos) { m._MousePosition = mouse_pos; }
        void SetIsLMousePressed(bool is_lmouse_pressed) { m._IsLMousePressed = is_lmouse_pressed; }
        void SetIsRMousePressed(bool is_rmouse_pressed) { m._IsRMousePressed = is_rmouse_pressed; }
        void SetIsWMousePressed(bool is_wmouse_pressed) { m._IsWMousePressed = is_wmouse_pressed; }
        void SetIsScrolling(bool is_scrolling) { m._IsScrolling = is_scrolling; }
    };
}