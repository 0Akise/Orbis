#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "Orbis/Base/Controls.hpp"
#include "Orbis/Base/ResourceVault.hpp"
#include "Orbis/Derma/Derma.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<std::shared_ptr<Derma>> mDermas;
        size_t mIDCounter;
        Controls mControls;
        ResourceVault mResourceVault;

        std::shared_ptr<Derma> Create_Instance(DType type, std::shared_ptr<Derma> parent) {
            auto derma = Derma::Create(type, mIDCounter++);

            derma->SetRegistered(true);

            mDermas.push_back(derma);

            if (parent != nullptr) {
                parent->AddChild(derma);
            }

            return derma;
        }

        std::shared_ptr<sf::Font> LoadFont_Instance(const std::string& path) {
            return mResourceVault.LoadFont(path);
        }

        std::shared_ptr<sf::Texture> LoadTexture_Instance(
            const std::string& path,
            bool srgb_enabled = false,
            const sf::IntRect& area = sf::IntRect()) {
            return mResourceVault.LoadTexture(path, srgb_enabled, area);
        }

        void ShowDermaList_Instance() {
            std::cout << "UI Dermas Listing\n";
            std::cout << "=====================\n";

            for (const auto& derma : mDermas) {
                std::cout << "ID: " << derma->GetID() << "\t"
                          << "Name: " << derma->GetName() << "\n";
            }

            std::cout << std::endl;
        }

        void Update_Instance(sf::RenderWindow& window) {
            mControls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            mControls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            mControls.mMouse.mLPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            mControls.mMouse.mRPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            mControls.mMouse.mWPress = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            for (auto& derma : mDermas) {
                derma->Update(mControls);
            }
        }

        void Render_Instance(sf::RenderWindow& window) {
            for (auto& derma : mDermas) {
                derma->Render(window);
            }
        }

    public:
        UI()
            : mDermas(),
              mIDCounter(0) {}

        static UI& GetUISystem() {
            static UI instance;

            return instance;
        }

        static std::shared_ptr<Derma> Create(DType type) {
            return GetUISystem().Create_Instance(type, nullptr);
        }

        static std::shared_ptr<Derma> Create(DType type, std::shared_ptr<Derma> parent) {
            return GetUISystem().Create_Instance(type, parent);
        }

        static std::shared_ptr<sf::Font> LoadFont(const std::string& path) {
            return GetUISystem().LoadFont_Instance(path);
        }

        static std::shared_ptr<sf::Texture> LoadTexture(
            const std::string& path,
            bool srgb_enabled = false,
            const sf::IntRect& area = sf::IntRect()) {
            return GetUISystem().LoadTexture_Instance(path, srgb_enabled, area);
        }

        static void ClearAllResources() {
            GetUISystem().mResourceVault.ClearAllResources();
        }

        static void ShowDermaList() {
            GetUISystem().ShowDermaList_Instance();
        }

        static void Update(sf::RenderWindow& window) {
            GetUISystem().Update_Instance(window);
        }

        static void Render(sf::RenderWindow& window) {
            GetUISystem().Render_Instance(window);
        }
    };
}