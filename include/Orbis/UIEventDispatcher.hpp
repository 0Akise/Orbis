#pragma once
#include <memory>

namespace Orbis {
    class DermaBase;

    class UIEventDispatcher {
    private:
        inline static UIEventDispatcher* mInstance = nullptr;

    public:
        virtual ~UIEventDispatcher() = default;
        virtual void NotifyZLevelChanged() = 0;
        virtual void NotifyDermaSelected(std::shared_ptr<DermaBase> derma) = 0;

        static UIEventDispatcher& Get() {
            if (mInstance == nullptr) {
                throw std::runtime_error("UIEventDispatcher not initialized");
            }

            return *mInstance;
        }

        static void SetInstance(UIEventDispatcher* instance) {
            mInstance = instance;
        }
    };
}