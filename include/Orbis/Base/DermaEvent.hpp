#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/System/Controls.hpp"

namespace Orbis {
    enum class EventType {
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

        VisibilityChanged,
    };

    struct DermaEvent {
        EventType mType;

        sf::Vector2f mPosition;
        sf::Vector2f mSize;
        size_t mZLevel;

        Controls mControls;

        bool mIsInBounds;
        bool mIsVisible;
    };

    using EventCallback = std::function<void(const DermaEvent&)>;
    using NotifyCallback = std::function<void(EventType, const void*)>;

    class DermaEventHandler {
    private:
        std::unordered_map<EventType, std::vector<EventCallback>> mListeners;

    public:
        void RegisterListener(EventType event_type, EventCallback callback) {
            mListeners[event_type].push_back(std::move(callback));
        }

        void DeregisterListener() {
            // do stuff
        }

        void EmitEvent(const DermaEvent& event) {
            auto iter = mListeners.find(event.mType);

            if ((iter != mListeners.end()) && (iter->second.empty() == false)) {
                for (const auto& callback : iter->second) {
                    callback(event);
                }
            }
        }
    };
}