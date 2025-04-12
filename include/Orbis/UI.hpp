#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Orbis/Derma.hpp"
#include "Orbis/Enums.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<Derma> mDermas;
        size_t mIDCounter;

    public:
        UI()
            : mDermas(),
              mIDCounter(0) {}

        Derma& Instance_Create(DermaType type) {
            Derma derma(type, mIDCounter++);

            derma.SetRegistered(true);
            mDermas.push_back(std::move(derma));

            return mDermas.back();
        }

        void Instance_ShowDermaList() {
            std::cout << "UI Dermas Listing\n";
            std::cout << "=====================\n";

            for (const auto& derma : mDermas) {
                std::cout << "ID: " << derma.GetID() << "\t"
                          << "Name: " << derma.GetName() << "\n";
            }

            std::cout << std::endl;
        }

        void Instance_Update(const Controls& controls) {
            for (auto& derma : mDermas) {
                derma.Update(controls);
            }
        }

        void Instance_Render(sf::RenderWindow& window) {
            for (auto& derma : mDermas) {
                derma.Render(window);
            }
        }

        static UI& GetUISystem() {
            static UI instance;

            return instance;
        }

        static Derma& Create(DermaType type) {
            return GetUISystem().Instance_Create(type);
        }

        static void ShowDermaList() {
            GetUISystem().Instance_ShowDermaList();
        }

        static void Update(const Controls& controls) {
            GetUISystem().Instance_Update(controls);
        }

        static void Render(sf::RenderWindow& window) {
            GetUISystem().Instance_Render(window);
        }
    };
}