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

            for (const auto& [id, drawing] : mDrawingsLine) {
                auto cloned_drawing = std::make_shared<DrawingsLine>(*drawing);

                cloned->mDrawingsLine[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing = std::make_shared<DrawingsRect>(*drawing);

                cloned->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing = std::make_shared<DrawingsText>(*drawing);

                cloned->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsWText) {
                auto cloned_drawing = std::make_shared<DrawingsWText>(*drawing);

                cloned->mDrawingsWText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing = std::make_shared<DrawingsTexture>(*drawing);

                cloned->mDrawingsTexture[id] = cloned_drawing;
            }

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
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