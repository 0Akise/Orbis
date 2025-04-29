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

    enum class DOption : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = Selectable | Movable | Resizable
    };

    inline DOption operator|(DOption a, DOption b) {
        return static_cast<DOption>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DOption operator&(DOption a, DOption b) {
        return static_cast<DOption>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
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
    };

    enum class DWindowState {
        None,
        Focused,
        Selected,
    };

    enum class DButtonState {
        Normal,
        Hover,
        Pressed,
    };
}