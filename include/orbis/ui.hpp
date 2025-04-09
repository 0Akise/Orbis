#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "orbis/controls.hpp"
#include "orbis/derma.hpp"

namespace Orbis {
    class UI {
    private:
        std::vector<Derma> mDermas;
        // size_t mIDFocused;
        size_t mIDCounter;

    public:
        UI() : mDermas(), mIDCounter(0) {}

        static UI Create() {
            return UI();
        }

        void DermaRegister(std::string name, sf::Vector2f size, sf::Vector2f pos) {
            mDermas.push_back(Derma::Create(mIDCounter, std::move(name), size, pos, 0));

            ++mIDCounter;
        }

        void DermaRemove(size_t id, const std::string& name) {
            auto iter = std::find_if(mDermas.begin(), mDermas.end(), [id, &name](const Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != mDermas.end()) {
                mDermas.erase(iter);
            }
        }

        void DermaGetInfo(size_t id, const std::string& name) {
            auto iter = std::find_if(mDermas.begin(), mDermas.end(), [id, &name](const Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != mDermas.end()) {
                std::cout << "ID: " << iter->GetID() << "\tName: " << iter->GetName() << "\n";
                std::cout << "Size: " << iter->GetSize().x << " " << iter->GetSize().y << "\n";
                std::cout << "Pos: " << iter->GetPosition().x << " " << iter->GetPosition().y << "\n";
                std::cout << "Z Level: " << iter->GetZLevel() << "\n";
                std::cout << "Debug: " << iter->GetDebugModeStatus() << "\n";
            }

            std::cout << std::endl;
        }

        void DermaShowList() {
            for (auto& derma : mDermas) {
                std::cout << "ID: " << derma.GetID() << "\tName: " << derma.GetName() << "\n";
            }

            std::cout << std::endl;
        }

        Derma* FindDerma(size_t id, const std::string& name) {
            auto iter = std::find_if(mDermas.begin(), mDermas.end(), [id, &name](const Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            return (iter != mDermas.end()) ? &(*iter) : nullptr;
        }

        template <typename F>
        void ApplyToDerma(size_t id, const std::string& name, F&& func) {
            auto iter = std::find_if(mDermas.begin(), mDermas.end(), [id, &name](const Derma& derma) {
                return derma.GetID() == id && derma.GetName() == name;
            });

            if (iter != mDermas.end()) {
                func(*iter);
            }
        }

        void DermaSetSelectable(size_t id, const std::string& name, bool is_selectable) {
            ApplyToDerma(id, name, [is_selectable](Derma& derma) {
                derma.SetSelectable(is_selectable);
            });
        }

        void DermaSetMovable(size_t id, const std::string& name, bool is_movable) {
            ApplyToDerma(id, name, [is_movable](Derma& derma) {
                if ((is_movable == true) && (derma.GetComponent<Selectable>() == nullptr))
                    derma.SetSelectable(true);

                derma.SetMovable(is_movable);
            });
        }

        void DermaSetResizable(size_t id, std::string name, bool is_resizable) {
            ApplyToDerma(id, name, [is_resizable](Derma& derma) {
                if ((is_resizable == true) && (derma.GetComponent<Selectable>() == nullptr))
                    derma.SetSelectable(true);

                derma.SetResizable(is_resizable);
            });
        }

        void DermaSetDebugMode(size_t id, std::string name, bool is_debug_mode) {
            ApplyToDerma(id, name, [is_debug_mode](Derma& derma) {
                derma.SetDebugMode(is_debug_mode);
            });
        }

        void Update(const Controls& controls) {
            for (auto& derma : mDermas) {
                derma.Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) {
            std::sort(mDermas.begin(), mDermas.end(), [](const Derma& a, const Derma& b) {
                return a.GetZLevel() < b.GetZLevel();
            });

            for (auto& derma : mDermas) {
                derma.Render(window);
            }
        }
    };
}