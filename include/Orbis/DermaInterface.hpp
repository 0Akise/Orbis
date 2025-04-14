#pragma once

#include <SFML/System.hpp>

namespace Orbis {

    class DermaInterface {
    public:
        virtual ~DermaInterface() = default;

        virtual sf::Vector2f GetPositionGlobal() const = 0;
    };
}