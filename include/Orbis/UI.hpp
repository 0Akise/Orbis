#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Orbis/Base/Derma.hpp"
#include "Orbis/Base/Widget.hpp"
#include "Orbis/Base/WidgetBase.hpp"
#include "Orbis/System/Controls.hpp"
#include "Orbis/System/ResourceVault.hpp"

namespace Orbis {
    class UIContext {
    private:
        Controls mControls;
        ResourceVault mResourceVault;

        std::vector<std::shared_ptr<Derma>> mDermas;
        size_t mDermaIDs = 0;

        std::weak_ptr<Derma> mSelectedDerma;
        bool mZLevelIsDirty = false;

    public:
        ResourceVault& AccessResourceVault() {
            return mResourceVault;
        }

        size_t GetDermaID() {
            return mDermaIDs;
        }

        void SetDermaID() {
            mDermaIDs++;
        }

        void NotifyZLevelChanged() {
            mZLevelIsDirty = true;
        }

        void NotifyDermaSelected(std::shared_ptr<Derma> derma) {
            auto previous = mSelectedDerma.lock();

            if ((previous != nullptr) && (previous != derma)) {
                mZLevelIsDirty = true;
            }

            mSelectedDerma = derma;
        }

        void RecalculateZLevels() {
            if (mZLevelIsDirty == false) {
                return;
            }

            mZLevelIsDirty = false;
        }

        void ShowDermaList() {
            std::cout << "UI Dermas Listing\n";
            std::cout << "=====================\n";

            for (auto& derma : mDermas) {
                std::cout << "ID: " << derma->GetID() << "\t"
                          << "Name: " << derma->GetName() << "\n";
            }

            std::cout << std::endl;
        }

        void Update(sf::RenderWindow& window) {
            mControls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            mControls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            mControls.mMouse.mLPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            mControls.mMouse.mRPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            mControls.mMouse.mWPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            for (auto& derma : mDermas) {
                derma->Update(mControls);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mZLevelIsDirty == true) {
                RecalculateZLevels();
            }

            for (auto& derma : mDermas) {
                derma->Render(window);
            }
        }
    };

    class UIManager {
    private:
        UIManager() = default;
        std::unordered_map<sf::RenderWindow*, UIContext*> mContexts;

    public:
        static UIManager& Get() {
            static UIManager instance;

            return instance;
        }

        static void Initialize() {
            Get();
        }

        void Bind(sf::RenderWindow& window, UIContext& context) {
            mContexts[&window] = &context;
        }

        UIContext* GetContext(sf::RenderWindow& window) {
            auto iter = mContexts.find(&window);

            return iter != mContexts.end() ? iter->second : nullptr;
        }
    };

    class UI {
    public:
        static inline std::vector<std::shared_ptr<WidgetInterface>> mWidgetStorage;

        static inline void Initialize() {
            UIManager::Initialize();
        }

        static inline void Bind(sf::RenderWindow& window, UIContext& context) {
            UIManager::Get().Bind(window, context);
        }

        static Derma& CreateDerma(UIContext& context) {
            auto derma = std::make_shared<Derma>();
            derma->SetID(context.GetDermaID());
            context.SetDermaID();

            return *derma;
        }

        template <WidgetType Type>
        static auto& CreateWidget() {
            if constexpr (Type == WidgetType::Button) {
                auto widget = std::make_shared<Button>();

                mWidgetStorage.push_back(widget);

                return *widget;
            } else {
                static_assert(Type == WidgetType::Button, "Unsupported Widget Type");
            }
        }

        static inline std::shared_ptr<sf::Font> LoadFont(UIContext& context, const std::string& path) {
            return context.AccessResourceVault().LoadFont(path);
        }

        static inline std::shared_ptr<sf::Texture> LoadTexture(
            UIContext& context,
            const std::string& path,
            bool srgb_enabled = false,
            const sf::IntRect& area = sf::IntRect()) {
            return context.AccessResourceVault().LoadTexture(path, srgb_enabled, area);
        }

        static inline void clearAllResources(UIContext& context) {
            context.AccessResourceVault().ClearAllResources();
        }

        static inline void ShowDermaList(UIContext& context) {
            context.ShowDermaList();
        }

        static inline void Update(sf::RenderWindow& window) {
            UIManager::Get().GetContext(window)->Update(window);
        }

        static inline void Render(sf::RenderWindow& window) {
            UIManager::Get().GetContext(window)->Render(window);
        }
    };
}