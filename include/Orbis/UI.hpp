#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "Orbis/Base/Panel.hpp"
#include "Orbis/Base/PanelHandle.hpp"
#include "Orbis/Base/Widget.hpp"
#include "Orbis/Base/WidgetBase.hpp"
#include "Orbis/Base/WidgetHandle.hpp"
#include "Orbis/System/Controls.hpp"
#include "Orbis/System/ResourceVault.hpp"

namespace Orbis {
    class UIContext {
    private:
        Controls mControls;
        ResourceVault mResourceVault;

        std::vector<std::shared_ptr<Panel>> mPanels;
        std::vector<std::shared_ptr<WidgetInterface>> mWidgets;
        size_t mPanelIDs = 0;

        std::weak_ptr<Panel> mSelectedPanel;
        bool mZLevelIsDirty = false;

    public:
        ResourceVault& AccessResourceVault() {
            return mResourceVault;
        }

        void AddPanel(std::shared_ptr<Panel> panel) {
            mPanels.push_back(panel);
        }

        template <typename T>
        void RegisterWidget(std::shared_ptr<T> widget) {
            mWidgets.push_back(std::static_pointer_cast<WidgetInterface>(widget));
        }

        template <WidgetType Type>
        auto CreateWidget() {
            if constexpr (Type == WidgetType::Button) {
                auto widget = std::make_shared<Button>();

                RegisterWidget(widget);

                return WidgetHandle<Button>(widget);
            }
        }

        size_t GetPanelID() {
            return mPanelIDs;
        }

        void SetPanelID() {
            mPanelIDs++;
        }

        void NotifyZLevelChanged() {
            mZLevelIsDirty = true;
        }

        void NotifySelected(std::shared_ptr<Panel> panel) {
            auto previous = mSelectedPanel.lock();

            if ((previous != nullptr) && (previous != panel)) {
                mZLevelIsDirty = true;
            }

            mSelectedPanel = panel;
        }

        void RecalculateZLevels() {
            if (mZLevelIsDirty == false) {
                return;
            }

            mZLevelIsDirty = false;
        }

        void ShowPanelList() {
            std::cout << "UI Panels Listing\n";
            std::cout << "=====================\n";

            for (auto& panel : mPanels) {
                std::cout << "ID: " << panel->GetID() << "\t"
                          << "Name: " << panel->GetName() << "\n";
            }

            std::cout << std::endl;
        }

        void Update(sf::RenderWindow& window) {
            mControls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            mControls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            mControls.mMouse.mLPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            mControls.mMouse.mRPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            mControls.mMouse.mWPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            for (auto& panel : mPanels) {
                panel->Update(mControls);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mZLevelIsDirty == true) {
                RecalculateZLevels();
            }

            for (auto& panel : mPanels) {
                panel->Render(window);
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
        static inline void Initialize() {
            UIManager::Initialize();
        }

        static inline void Bind(sf::RenderWindow& window, UIContext& context) {
            UIManager::Get().Bind(window, context);
        }

        static PanelHandle CreatePanel(UIContext& context) {
            auto panel = std::make_shared<Panel>();

            panel->SetID(context.GetPanelID());
            context.SetPanelID();
            context.AddPanel(panel);

            return PanelHandle(panel);
        }

        template <WidgetType Type>
        static auto CreateWidget() {
            if constexpr (Type == WidgetType::Panel) {
                auto widget = std::make_shared<Panel>();

                return WidgetHandle<Panel>(widget);
            } else if constexpr (Type == WidgetType::Button) {
                auto widget = std::make_shared<Button>();

                return WidgetHandle<Button>(widget);
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

        static inline void ShowPanelList(UIContext& context) {
            context.ShowPanelList();
        }

        static inline void Update(sf::RenderWindow& window) {
            UIManager::Get().GetContext(window)->Update(window);
        }

        static inline void Render(sf::RenderWindow& window) {
            UIManager::Get().GetContext(window)->Render(window);
        }
    };
}