#pragma once

#include <SFML/System.hpp>

#include <type_traits>

namespace Orbis {
    class Movable {
    public:
        virtual ~Movable() = default;
        virtual void MoveStart(sf::Vector2i delta_mouse) = 0;
        virtual void MoveEnd() = 0;
        virtual void OnMove() = 0;
    };

    class Resizable {
    public:
        virtual ~Resizable() = default;
        virtual void ResizeStart(sf::Vector2i delta_mouse) = 0;
        virtual void ResizeEnd() = 0;
        virtual void OnResize() = 0;
    };

    class Customizable {
    public:
        virtual ~Customizable() = default;
    };

    class Default {
    public:
        virtual ~Default() = default;
    };
}