#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "component.hpp"
#include "sfml/shapes.hpp"

namespace Orbis {
    class Derma {
    private:
        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;
        bool mIsVisible;
        bool mIsDebugMode;

        std::vector<std::unique_ptr<DermaComponent>> mComponents;

        Selectable* mComponentSelectable = nullptr;
        Movable* mComponentMovable = nullptr;
        Resizable* mComponentResizable = nullptr;

    public:
        Derma(size_t id, std::string name, sf::Vector2f size, sf::Vector2f position, size_t z_level)
            : mID(id), mName(std::move(name)), mSize(size), mPosition(position), mZLevel(z_level), mIsVisible(true), mIsDebugMode(false) {}

        static Derma Create(size_t id, std::string name, sf::Vector2f size, sf::Vector2f position, size_t z_level) {
            return Derma(id, std::move(name), size, position, z_level);
        }

        size_t GetID() const { return mID; }
        const std::string& GetName() const { return mName; }
        sf::Vector2f GetSize() const { return mSize; }
        sf::Vector2f GetPosition() const { return mPosition; }
        size_t GetZLevel() const { return mZLevel; }
        std::string GetDebugModeStatus() const { return (mIsDebugMode == true ? "true" : "false"); }

        template <typename T>
        T* GetComponent() const {
            for (const auto& component : mComponents) {
                auto* casted = dynamic_cast<T*>(component.get());

                if (casted != nullptr) {
                    return casted;
                }
            }

            return nullptr;
        }

        void SetName(std::string name) { mName = std::move(name); }
        void SetSize(sf::Vector2f size) { mSize = size; }
        void SetPosition(sf::Vector2f position) { mPosition = position; }
        void SetZLevel(size_t zlevel) { mZLevel = zlevel; }
        void SetDebugMode(bool is_debugmode) { mIsDebugMode = is_debugmode; }

        template <typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            auto component = std::make_unique<T>(*this, std::forward<Args>(args)...);
            T* raw_pointer = component.get();
            mComponents.push_back(std::move(component));

            if constexpr (std::is_same_v<T, Selectable>) {
                mComponentSelectable = raw_pointer;
            }

            if constexpr (std::is_same_v<T, Movable>) {
                mComponentMovable = raw_pointer;
            }

            if constexpr (std::is_same_v<T, Resizable>) {
                mComponentResizable = raw_pointer;
            }

            return raw_pointer;
        }

        template <typename T>
        void RemoveComponent() {
            mComponents.erase(
                std::remove_if(mComponents.begin(), mComponents.end(), [](const auto& component) {
                    return dynamic_cast<T*>(component.get()) != nullptr;
                }),
                mComponents.end());

            if constexpr (std::is_same_v<T, Selectable>) {
                mComponentSelectable = nullptr;
            }

            if constexpr (std::is_same_v<T, Movable>) {
                mComponentMovable = nullptr;
            }

            if constexpr (std::is_same_v<T, Resizable>) {
                mComponentResizable = nullptr;
            }
        }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (mComponentSelectable == nullptr)) {
                mComponentSelectable = AddComponent<Selectable>();
            } else if ((is_selectable == false) && (mComponentSelectable != nullptr)) {
                RemoveComponent<Selectable>();

                mComponentSelectable = nullptr;
            }
        };

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (mComponentMovable == nullptr)) {
                mComponentMovable = AddComponent<Movable>();
            } else if ((is_movable == false) && (mComponentMovable != nullptr)) {
                RemoveComponent<Movable>();

                mComponentMovable = nullptr;
            }
        };

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (mComponentResizable == nullptr)) {
                mComponentResizable = AddComponent<Resizable>();
            } else if ((is_resizable == false) && (mComponentResizable != nullptr)) {
                RemoveComponent<Resizable>();

                mComponentResizable = nullptr;
            }
        };

        void Update(const Controls& controls);
        void Render(sf::RenderWindow& window);
    };
}