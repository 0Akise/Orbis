#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

#include "Orbis/Base/Controls.hpp"
#include "Orbis/Base/ResourceVault.hpp"
#include "Orbis/Derma/Derma.hpp"
#include "Orbis/Derma/DermaWidgets.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<std::shared_ptr<DermaBase>> mDermas;
        size_t mIDCounter;
        Controls mControls;
        ResourceVault mResourceVault;

        template <typename T, typename... Args>
        std::shared_ptr<T> MakeWidget(Args&&... args) {
            auto pointer = std::make_shared<T>(mIDCounter++, std::forward<Args>(args)...);
            pointer->Initialize();
            return pointer;
        }

    public:
        UI()
            : mDermas(),
              mIDCounter(0) {}

        static UI& GetUISystem() {
            static UI instance;

            return instance;
        }

        template <typename T>
        static T& Create() {
            auto pointer = GetUISystem().MakeWidget<T>();

            GetUISystem().mDermas.emplace_back(pointer);

            return *pointer;
        }

        template <typename T, typename Parent>
        static T& CreateChild(Parent& parent) {
            auto pointer_parent = parent.template GetSharedFromThis<Parent>();
            auto pointer = GetUISystem().MakeWidget<T>();

            pointer->SetParent(pointer_parent);
            pointer_parent->AddChild(pointer);

            GetUISystem().mDermas.emplace_back(pointer);

            return *pointer;
        }

        static std::shared_ptr<sf::Font> LoadFont(const std::string& path) {
            return GetUISystem().mResourceVault.LoadFont(path);
        }

        static std::shared_ptr<sf::Texture> LoadTexture(
            const std::string& path,
            bool srgb_enabled = false,
            const sf::IntRect& area = sf::IntRect()) {
            return GetUISystem().mResourceVault.LoadTexture(path, srgb_enabled, area);
        }

        static void ClearAllResources() {
            GetUISystem().mResourceVault.ClearAllResources();
        }

        static void ShowDermaList() {
            std::cout << "UI Dermas Listing\n";
            std::cout << "=====================\n";

            for (auto& derma : GetUISystem().mDermas) {
                std::cout << "ID: " << derma->GetID() << "\t"
                          << "Name: " << derma->GetName() << "\n";
            }

            std::cout << std::endl;
        }

        static void Update(sf::RenderWindow& window) {
            GetUISystem().mControls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            GetUISystem().mControls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            GetUISystem().mControls.mMouse.mLPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            GetUISystem().mControls.mMouse.mRPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            GetUISystem().mControls.mMouse.mWPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            for (auto& derma : GetUISystem().mDermas) {
                derma->Update(GetUISystem().mControls);
            }
        }

        static void Render(sf::RenderWindow& window) {
            for (auto& derma : GetUISystem().mDermas) {
                derma->Render(window);
            }
        }
    };
}