#pragma once

#include <cstdint>

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

    inline DermaOptionFlag operator|(DermaOptionFlag a, DermaOptionFlag b) {
        return static_cast<DermaOptionFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DermaOptionFlag operator&(DermaOptionFlag a, DermaOptionFlag b) {
        return static_cast<DermaOptionFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
}