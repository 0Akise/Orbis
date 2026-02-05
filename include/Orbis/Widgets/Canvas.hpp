#pragma once

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/Widgets/Widget.hpp"

namespace Orbis {
    class Canvas : public Widget {
    public:
        Canvas() = default;

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<Canvas>();

            cloned->mSize      = mSize;
            cloned->mPosition  = mPosition;
            cloned->mZLevel    = mZLevel;
            cloned->mIsVisible = mIsVisible;

            CloneDrawingsTo(cloned.get());

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            UpdateAnimation();

            (void)controls;
            (void)pos_panel;
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f pos_global = pos_panel + mPosition;

            RenderAllDrawings(window, pos_global);
        }
    };
} // namespace Orbis