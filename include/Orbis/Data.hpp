#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

namespace Orbis {
    enum class DType {
        DFrame,
        DWindow,
        DButton,
    };

    enum class DOptionFlag : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = Selectable | Movable | Resizable
    };

    inline DOptionFlag operator|(DOptionFlag a, DOptionFlag b) {
        return static_cast<DOptionFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DOptionFlag operator&(DOptionFlag a, DOptionFlag b) {
        return static_cast<DOptionFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    enum class DEventType {
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

    enum class DDrawingsType {
        Box,
        Image,
    };

    struct MouseState {
        sf::Vector2f mPosition = {0, 0};
        bool mLPress = false;
        bool mRPress = false;
        bool mWPress = false;
        bool mScroll = false;
    };

    struct DEvent {
        DEventType mType;

        sf::Vector2f mPosition;
        sf::Vector2f mSize;
        size_t mZLevel;

        MouseState mMouseState;

        bool mIsInBounds;
        bool mIsVisible;
    };

    using EventCallback = std::function<void(const DEvent&)>;
    using NotifyCallback = std::function<void(DEventType, const void*)>;
}