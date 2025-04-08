#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/System.hpp>

#include "controls.hpp"
#include "derma.hpp"

namespace Orbis {
    class DermaComponent {
    protected:
        Derma& mOwner;

    public:
        explicit DermaComponent(Derma& owner) : mOwner(owner) {}
        virtual ~DermaComponent() = default;

        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow& window) {}
    };

    class Selectable : public DermaComponent {
    private:
        bool mIsSelected;

    public:
        explicit Selectable(Derma& owner) : DermaComponent(owner) {}

        bool GetIsSelected() const { return mIsSelected; }

        void Update(const Controls& controls) override {}
        void Render(sf::RenderWindow& window) override {}
    };

    class Movable : public DermaComponent {
    private:
        bool mIsMoving;
        sf::Vector2f mOffsetMoving;

    public:
        explicit Movable(Derma& owner) : DermaComponent(owner) {}

        bool GetIsMoving() const { return mIsMoving; }

        void Update(const Controls& controls) override {}
        void Render(sf::RenderWindow& window) override {}
    };

    class Resizable : public DermaComponent {
    private:
        bool mIsResizing = false;
        sf::Vector2f mOffsetResizing;
        sf::Vector2f mMinimumSize = {10.0f, 10.0f};

    public:
        explicit Resizable(Derma& owner) : DermaComponent(owner) {}

        bool GetIsResizing() const { return mIsResizing; }

        void SetMinimumSize(sf::Vector2f size) { mMinimumSize = size; }

        void Update(const Controls& controls) override {}
        void Render(sf::RenderWindow& window) override {}
    };
}