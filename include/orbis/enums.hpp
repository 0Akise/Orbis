#pragma once

#include <cstdint>

namespace Orbis {
    enum class DermaType {
        DFrame,
        DWindow,
        DButton,
    };

    enum class DermaComponentFlag : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = None,
        Complete = Selectable | Movable | Resizable
    };

    inline DermaComponentFlag operator|(DermaComponentFlag a, DermaComponentFlag b) {
        return static_cast<DermaComponentFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DermaComponentFlag operator&(DermaComponentFlag a, DermaComponentFlag b) {
        return static_cast<DermaComponentFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
}