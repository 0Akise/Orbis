#pragma once
#include <memory>

namespace Orbis {
    class DermaBase;

    class UIEventDispatcher {
    public:
        virtual ~UIEventDispatcher() = default;
        virtual void NotifyZLevelChanged() = 0;
        virtual void NotifyDermaSelected(std::shared_ptr<DermaBase> derma) = 0;

        static UIEventDispatcher& Get() {
            static UIEventDispatcher* instance = nullptr;

            if (instance == nullptr) {
                throw std::runtime_error("UIEventDispatcher not initialized");
            }

            return *instance;
        }

        static void SetInstance(UIEventDispatcher* instance) {
            static UIEventDispatcher** instancePtr = []() {
                static UIEventDispatcher* ptr = nullptr;

                return &ptr;
            }();

            if (instance != nullptr) {
                *instancePtr = instance;
            }
        }
    };
}