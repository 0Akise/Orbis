#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

#include "Orbis/Base/Controls.hpp"
#include "Orbis/Base/ResourceVault.hpp"
#include "Orbis/Derma/Derma.hpp"
#include "Orbis/Derma/DermaComponents.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<std::shared_ptr<DermaInterface>> mDermas;
        size_t mIDCounter;
        Controls mControls;
        ResourceVault mResourceVault;

        template <typename T>
        std::shared_ptr<T> Create_Instance(DType type, std::shared_ptr<DermaInterface> parent) {
            std::shared_ptr<T> derma;

            switch (type) {
                case DType::DFrame: {
                    if constexpr (std::is_same_v<T, DFrame>) {
                        auto frame = std::make_shared<DFrame>(mIDCounter++);

                        frame->Initialize();
                        derma = frame;
                    } else {
                        throw std::runtime_error("Type mismatch: Expected DFrame");
                    }
                    break;
                }
                case DType::DWindow: {
                    if constexpr (std::is_same_v<T, DWindow>) {
                        auto window = std::make_shared<DWindow>(mIDCounter++);

                        window->Initialize();
                        derma = window;
                    } else {
                        throw std::runtime_error("Type mismatch: Expected DWindow");
                    }
                    break;
                }
                case DType::DButton: {
                    if constexpr (std::is_same_v<T, DButton>) {
                        auto button = std::make_shared<DButton>(mIDCounter++);

                        button->Initialize();
                        derma = button;
                    } else {
                        throw std::runtime_error("Type mismatch: Expected DButton");
                    }
                    break;
                }
                default: {
                    throw std::runtime_error("Unsupported component type");

                    break;
                }
            }

            if (derma == nullptr) {
                throw std::runtime_error("Failed to create component");
            }

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

        template <typename T>
        static T& Create() {
            static_assert(std::is_base_of<DermaInterface, T>::value, "T must derive from DermaInterface");

            auto pointer = GetUISystem().Create_Instance<T>(DTypeMap<T>::value, nullptr);

            return *pointer;
        }

        template <typename T, typename ParentType>
        static T& CreateChild(ParentType& parent) {
            static_assert(std::is_base_of<DermaInterface, T>::value, "T must derive from DermaInterface");
            static_assert(std::is_base_of<DermaInterface, ParentType>::value, "Parent must derive from DermaInterface");

            auto pointer_parent = parent.template GetSharedFromThis<ParentType>();
            auto pointer = GetUISystem().Create_Instance<T>(DTypeMap<T>::value, pointer_parent);

            return static_cast<T&>(*pointer);
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