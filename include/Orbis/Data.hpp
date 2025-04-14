#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

namespace Orbis {
    enum class DermaType {
        DFrame,
        DWindow,
        DButton,
    };

    enum class DermaOptionFlag : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = Selectable | Movable | Resizable
    };

    enum class DermaEventType {
        MouseEnter,
        MouseLeave,
        MouseDown,
        MouseLDown,
        MouseRDown,
        MouseUp,
        MouseLUp,
        MouseRUp,
        MouseMove,
        MouseClick,

        KeyDown,
        KeyUp,

        Selected,
        Moved,
        Resized,

        VisibilityChanged,

        ParentChanged,
        ChildAdded,
        ChildRemoved
    };

    struct DermaEvent {
        DermaEventType mType;

        sf::Vector2f mPosition;
        sf::Vector2f mSize;
        sf::Vector2f mPositionMouse;
        size_t mZLevel;

        bool mIsInBounds;
        bool mIsLMousePressed;
        bool mIsRMousePressed;
        bool mIsWMousePressed;
        bool mIsVisible;
    };

    using EventCallback = std::function<void(const DermaEvent&)>;
    using NotifyCallback = std::function<void(DermaEventType, const void*)>;

    inline DermaOptionFlag operator|(DermaOptionFlag a, DermaOptionFlag b) {
        return static_cast<DermaOptionFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DermaOptionFlag operator&(DermaOptionFlag a, DermaOptionFlag b) {
        return static_cast<DermaOptionFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
}