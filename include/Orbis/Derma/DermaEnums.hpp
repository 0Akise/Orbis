#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

#include "Orbis/Base/Controls.hpp"

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
        Rect,
        Text,
        Image,  // texture
        Sprite,
    };
}