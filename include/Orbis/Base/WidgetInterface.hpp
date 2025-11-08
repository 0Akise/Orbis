#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Controls.hpp"

namespace Orbis {
    class WidgetInterface : public std::enable_shared_from_this<WidgetInterface> {
    public:
        virtual ~WidgetInterface() = default;

        virtual void SetPositionOffset(sf::Vector2f offset) = 0;
        virtual sf::Vector2f GetPositionGlobal() const = 0;

        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow& window) = 0;
    };
}