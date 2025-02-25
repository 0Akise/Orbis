#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "interface.hpp"
#include "sfml/shapes.hpp"

#include <iostream>
#include <memory>

namespace Orbis {
    class Derma {
    private:
        struct M {
            size_t _ID;
            std::string _Name;
            sf::Vector2f _Size;
            sf::Vector2f _Position;
            size_t _ZLevel;

            bool _IsDraggable;
            bool _IsResizable;

            bool _IsSelected;
            bool _IsFocused;
            bool _IsHidden;

            bool _IsDragging;
            bool _IsResizing;

            sf::Vector2f _OffsetDragging;
            sf::Vector2f _OffsetResizing;

            bool _IsDebugMode;
        } m;

        explicit Derma(M m) : m(std::move(m)) {}

    public:
        static Derma Create(size_t id, std::string name, sf::Vector2f size, sf::Vector2f position) {
            return Derma(M{
                ._ID = id,
                ._Name = name,
                ._Size = size,
                ._Position = position,
                ._ZLevel = 0,

                ._IsDraggable = false,
                ._IsResizable = false,

                ._IsSelected = false,
                ._IsFocused = false,
                ._IsHidden = false,

                ._IsDragging = false,
                ._IsResizing = false,

                ._OffsetDragging = {0.0, 0.0},
                ._OffsetResizing = {0.0, 0.0},

                ._IsDebugMode = false});
        }

        size_t GetID() { return m._ID; }
        std::string GetName() { return m._Name; }
        sf::Vector2f GetSize() { return m._Size; }
        sf::Vector2f GetPosition() { return m._Position; }
        size_t GetZLevel() { return m._ZLevel; }
        std::string GetDebugModeStatus() { return (m._IsDebugMode == true ? "true" : "false"); }

        void SetName(std::string name) { m._Name = name; }
        void SetSize(sf::Vector2f size) { m._Size = size; }
        void SetPosition(sf::Vector2f position) { m._Position = position; }
        void SetZLevel(size_t zvalue) { m._ZLevel = zvalue; }
        void SetDraggable(bool is_draggable) { m._IsDraggable = is_draggable; }
        void SetResizable(bool is_resizable) { m._IsResizable = is_resizable; }
        void SetDebugMode(bool is_debugmode) { m._IsDebugMode = is_debugmode; }

        bool IsMouseInDermaArea(sf::Vector2i pos_mouse) {
            float p_mx = static_cast<float>(pos_mouse.x);
            float p_my = static_cast<float>(pos_mouse.y);
            float p_x1 = m._Position.x;
            float p_x2 = m._Position.x + m._Size.x;
            float p_y1 = m._Position.y;
            float p_y2 = m._Position.y + m._Size.y;

            return ((p_x1 <= p_mx && p_mx <= p_x2) && (p_y1 <= p_my && p_my <= p_y2)) ? true : false;
        }

        void HandleMouseClicks(sf::Vector2i pos_mouse, bool is_lmouse_pressed, bool is_rmouse_pressed) {
            float p_mx = static_cast<float>(pos_mouse.x);
            float p_my = static_cast<float>(pos_mouse.y);
            sf::Vector2f pos_mouse_f = {p_mx, p_my};

            if ((is_lmouse_pressed == true) || (is_rmouse_pressed == true)) {
                if (IsMouseInDermaArea(pos_mouse) == true) {
                    m._IsSelected = true;
                }

                if ((m._IsSelected == true) && (m._IsDraggable == true) && (m._IsDragging == false) && (is_rmouse_pressed == false)) {
                    m._IsDragging = true;
                    m._OffsetDragging = pos_mouse_f - m._Position;
                }

                if ((m._IsSelected == true) && (m._IsResizable == true) && (m._IsResizing == false) && (is_lmouse_pressed == false)) {
                    m._IsResizing = true;
                    m._OffsetResizing = pos_mouse_f - m._Size;
                }

                if (m._IsDragging == true) {
                    m._Position = pos_mouse_f - m._OffsetDragging;
                }

                if (m._IsResizing == true) {
                    m._Size = pos_mouse_f - m._OffsetResizing;
                }
            } else {
                m._IsSelected = false;
                m._IsDragging = false;
                m._IsResizing = false;
            }
        };

        void Render(sf::RenderWindow& window) {
            if (m._IsDebugMode == true) {
                sf::RectangleShape debug_rectangle(m._Size);

                debug_rectangle.setPosition(m._Position);
                debug_rectangle.setFillColor(sf::Color(255, 255, 255, 255));

                if (m._IsSelected == true) {
                    debug_rectangle.setFillColor(sf::Color(100, 100, 100, 255));
                }

                window.draw(debug_rectangle);
            }
        }
    };
}