#pragma once

#include <SFML/Graphics.hpp>

#include "controls.hpp"
#include "derma.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

namespace Orbis {
    class UI {
    private:
        struct M {
            std::vector<Derma> _Dermas;
            size_t _IDFocused;
            size_t _IDCounter;
        } m;

        explicit UI(M m) : m(std::move(m)) {}

    public:
        static UI Create() {
            return UI(M{
                ._Dermas = {},
                ._IDFocused = 0,
                ._IDCounter = 0});
        }

        void DermaRegister(std::string name, sf::Vector2f size, sf::Vector2f pos) {
            m._Dermas.push_back(Derma::Create(m._IDCounter, name, size, pos));

            ++m._IDCounter;
        }

        void DermaRemove(size_t id, std::string name) {
            auto iter = std::find_if(m._Dermas.begin(), m._Dermas.end(), [id, &name](Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != m._Dermas.end()) {
                m._Dermas.erase(iter);
            }
        }

        void DermaGetInfo(size_t id, std::string name) {
            auto iter = std::find_if(m._Dermas.begin(), m._Dermas.end(), [id, &name](Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != m._Dermas.end()) {
                std::cout << "ID: " << iter->GetID() << "\tName: " << iter->GetName() << "\n";
                std::cout << "Size: " << iter->GetSize().x << " " << iter->GetSize().y << "\n";
                std::cout << "Pos: " << iter->GetPosition().x << " " << iter->GetPosition().y << "\n";
                std::cout << "Z Level: " << iter->GetZLevel() << "\n";
                std::cout << "Debug: " << iter->GetDebugModeStatus() << "\n";
            }

            std::cout << std::endl;
        }

        void DermaShowList() {
            for (auto& derma : m._Dermas) {
                std::cout << "ID: " << derma.GetID() << "\tName: " << derma.GetName() << "\n";
            }

            std::cout << std::endl;
        }

        void DermaSetDraggable(size_t id, std::string name, bool is_draggable) {
            auto iter = std::find_if(m._Dermas.begin(), m._Dermas.end(), [id, &name](Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != m._Dermas.end()) {
                iter->SetDraggable(is_draggable);
            }
        }

        void DermaSetResizable(size_t id, std::string name, bool is_resizable) {
            auto iter = std::find_if(m._Dermas.begin(), m._Dermas.end(), [id, &name](Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != m._Dermas.end()) {
                iter->SetResizable(is_resizable);
            }
        }

        void DermaSetDebugMode(size_t id, std::string name, bool is_debugmode) {
            auto iter = std::find_if(m._Dermas.begin(), m._Dermas.end(), [id, &name](Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != m._Dermas.end()) {
                iter->SetDebugMode(is_debugmode);
            }
        }

        void Update(Controls& control) {
            for (auto& derma : m._Dermas) {
                derma.HandleMouseClicks(control.GetMousePosition(), control.GetIsLMousePressed(), control.GetIsRMousePressed());
            }
        }

        void Render(sf::RenderWindow& window) {
            for (auto& derma : m._Dermas) {
                derma.Render(window);
            }
        }
    };
}