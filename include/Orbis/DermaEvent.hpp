#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Data.hpp"

namespace Orbis {
    class EventSystem {
    private:
        std::unordered_map<DermaEventType, std::vector<EventCallback>> mListeners;

    public:
        void RegisterListener(DermaEventType event_type, EventCallback callback) {
            mListeners[event_type].push_back(std::move(callback));
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