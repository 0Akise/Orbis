#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace Orbis {
    enum class DermaEventType {
        MouseEnter,
        MouseLeave,
        MouseDown,
        MouseUp,
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

    class EventSystem {
    public:
        using EventCallback = std::function<void(const DermaEvent&)>;

        void RegisterListener(DermaEventType event_type, EventCallback callback) {
            mListeners[event_type].push_back(std::move(callback));
        }

        void EmitEvent(const DermaEvent& event) {
            auto iter = mListeners.find(event.mType);

            if (iter != mListeners.end()) {
                for (const auto& callback : iter->second) {
                    callback(event);
                }
            }
        }

    private:
        std::unordered_map<DermaEventType, std::vector<EventCallback>> mListeners;
    };
}