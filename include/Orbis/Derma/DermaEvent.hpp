#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Base/Controls.hpp"
#include "Orbis/Derma/DermaEnums.hpp"

namespace Orbis {
    struct DEvent {
        DEventType mType;

        sf::Vector2f mPosition;
        sf::Vector2f mSize;
        size_t mZLevel;

        Controls mControls;

        bool mIsInBounds;
        bool mIsVisible;
    };

    using EventCallback = std::function<void(const DEvent&)>;
    using NotifyCallback = std::function<void(DEventType, const void*)>;

    class DermaEventSystem {
    private:
        std::unordered_map<DEventType, std::vector<EventCallback>> mListeners;

    public:
        void RegisterListener(DEventType event_type, EventCallback callback) {
            mListeners[event_type].push_back(std::move(callback));
        }

        void EmitEvent(const DEvent& event) {
            auto iter = mListeners.find(event.mType);

            if ((iter != mListeners.end()) && (iter->second.empty() == false)) {
                for (const auto& callback : iter->second) {
                    callback(event);
                }
            }
        }
    };
}