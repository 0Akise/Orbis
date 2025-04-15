#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Orbis/Data.hpp"
#include "Orbis/Derma.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<std::shared_ptr<Derma>> mDermas;
        size_t mIDCounter;

    public:
        UI()
            : mDermas(),
              mIDCounter(0) {}

        static UI& GetUISystem() {
            static UI instance;

            return instance;
        }

        std::shared_ptr<Derma> Instance_Create(DermaType type, std::shared_ptr<Derma> parent) {
            auto derma = Derma::Create(type, mIDCounter++);

            derma->SetRegistered(true);

            mDermas.push_back(derma);

            if (parent) {
                parent->AddChild(derma);
            }

            return derma;
        }

        static std::shared_ptr<Derma> Create(DermaType type) {
            return GetUISystem().Instance_Create(type, nullptr);
        }

        static std::shared_ptr<Derma> Create(DermaType type, std::shared_ptr<Derma> parent) {
            return GetUISystem().Instance_Create(type, parent);
        }

        void Instance_ShowDermaList() {
            std::cout << "UI Dermas Listing\n";
            std::cout << "=====================\n";

            for (const auto& derma : mDermas) {
                std::cout << "ID: " << derma->GetID() << "\t"
                          << "Name: " << derma->GetName() << "\n";
            }

            std::cout << std::endl;
        }

        static void ShowDermaList() {
            GetUISystem().Instance_ShowDermaList();
        }

        void Instance_Update(const Controls& controls) {
            for (auto& derma : mDermas) {
                derma->Update(controls);
            }
        }

        static void Update(const Controls& controls) {
            GetUISystem().Instance_Update(controls);
        }

        void Instance_Render(sf::RenderWindow& window) {
            for (auto& derma : mDermas) {
                derma->Render(window);
            }
        }

        static void Render(sf::RenderWindow& window) {
            GetUISystem().Instance_Render(window);
        }
    };
}