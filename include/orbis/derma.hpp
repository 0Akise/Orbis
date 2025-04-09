#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "sfml/shapes.hpp"

namespace Orbis {
    class DermaComponent;
    class Selectable;
    class Movable;
    class Resizable;
    class Derma;
}

namespace Orbis {
    class DermaComponent {
    protected:
        Derma& mOwner;

    public:
        explicit DermaComponent(Derma& owner) : mOwner(owner) {}
        virtual ~DermaComponent() = default;

        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow&) {};
    };

    class Derma {
    private:
        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        bool mNeedsZLevelSorting = false;
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

        bool ContainsCursor(sf::Vector2f cursor) const {
            return ((mPosition.x <= cursor.x) && (cursor.x <= mPosition.x + mSize.x)) &&
                   ((mPosition.y <= cursor.y) && (cursor.y <= mPosition.y + mSize.y));
        }

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

        size_t GetID() const { return mID; }
        const std::string& GetName() const { return mName; }
        sf::Vector2f GetSize() const { return mSize; }
        sf::Vector2f GetPosition() const { return mPosition; }

        size_t GetZLevel() const { return mZLevel; }

        bool GetDebugModeStatus() const { return mIsDebugMode; }

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

        void SetZLevel(size_t zlevel) {
            if (mZLevel != zlevel) {
                mZLevel = zlevel;
                mNeedsZLevelSorting = true;
            }
        }

        void SetDebugMode(bool is_debugmode) { mIsDebugMode = is_debugmode; }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (mComponentSelectable == nullptr)) {
                mComponentSelectable = AddComponent<Selectable>();
            } else if ((is_selectable == false) && (mComponentSelectable != nullptr)) {
                RemoveComponent<Selectable>();

                mComponentSelectable = nullptr;
            }
        }

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (mComponentMovable == nullptr)) {
                mComponentMovable = AddComponent<Movable>();
            } else if ((is_movable == false) && (mComponentMovable != nullptr)) {
                RemoveComponent<Movable>();

                mComponentMovable = nullptr;
            }
        }

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (mComponentResizable == nullptr)) {
                mComponentResizable = AddComponent<Resizable>();
            } else if ((is_resizable == false) && (mComponentResizable != nullptr)) {
                RemoveComponent<Resizable>();

                mComponentResizable = nullptr;
            }
        }

        void Update(const Controls& controls) {
            if (mIsVisible == false)
                return;

            for (auto& component : mComponents) {
                component->Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsVisible == false)
                return;

            if (mIsDebugMode == true) {
                sf::RectangleShape rect(mSize);
                rect.setPosition(mPosition);
                rect.setFillColor(sf::Color(255, 255, 255, 128));
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1.0f);

                window.draw(rect);
            }

            for (auto& component : mComponents) {
                component->Render(window);
            }
        }
    };

    class Selectable : public DermaComponent {
    private:
        bool mIsSelected;

    public:
        explicit Selectable(Derma& owner) : DermaComponent(owner) {}

        bool GetSelectedStatus() const { return mIsSelected; }

        void Update(const Controls& controls) override {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            bool is_lmouse_pressed = controls.GetIsLMousePressed();

            if (is_lmouse_pressed == true) {
                if ((mIsSelected == false) && mOwner.ContainsCursor(pos_mouse_f)) {
                    mIsSelected = true;
                } else {
                    mIsSelected = false;
                }
            }
        }
    };

    class Movable : public DermaComponent {
    private:
        bool mIsMoving;
        sf::Vector2f mOffsetMoving;

    public:
        explicit Movable(Derma& owner) : DermaComponent(owner) {}

        bool GetMovingStatus() const { return mIsMoving; }

        void Update(const Controls& controls) override {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            bool is_lmouse_pressed = controls.GetIsLMousePressed();

            if (is_lmouse_pressed) {
                if (mIsMoving == false && mOwner.ContainsCursor(pos_mouse_f)) {
                    mIsMoving = true;
                    mOffsetMoving = pos_mouse_f - mOwner.GetPosition();
                }

                if (mIsMoving == true) {
                    mOwner.SetPosition(pos_mouse_f - mOffsetMoving);
                }
            } else {
                mIsMoving = false;
            }
        }
    };

    class Resizable : public DermaComponent {
    private:
        bool mIsResizing = false;
        sf::Vector2f mOffsetResizing;
        sf::Vector2f mMinimumSize = {10.0f, 10.0f};

    public:
        explicit Resizable(Derma& owner) : DermaComponent(owner) {}

        bool GetResizingStatus() const { return mIsResizing; }
        sf::Vector2f GetMinimumSize() const { return mMinimumSize; }

        void SetMinimumSize(sf::Vector2f size) { mMinimumSize = size; }

        void Update(const Controls& controls) override {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            bool is_rmouse_pressed = controls.GetIsRMousePressed();

            sf::Vector2f size_owner = mOwner.GetSize();
            sf::Vector2f pos_owner = mOwner.GetPosition();
            sf::Vector2f br_owner = pos_owner + size_owner;

            sf::Vector2f size_handle = {10.0f, 10.0f};
            sf::Vector2f pos_handle = {br_owner.x - size_handle.x, br_owner.y - size_handle.y};
            sf::Rect<float> handle(pos_handle, size_handle);
            bool is_in_handle = handle.contains(pos_mouse_f);

            if (is_rmouse_pressed == true) {
                if ((mIsResizing == false) && is_in_handle) {
                    mIsResizing = true;
                    mOffsetResizing = pos_mouse_f - br_owner;
                }

                if (mIsResizing == true) {
                    sf::Vector2f size_new = (pos_mouse_f - mOffsetResizing) - pos_owner;

                    size_new.x = std::max(size_new.x, mMinimumSize.x);
                    size_new.y = std::max(size_new.y, mMinimumSize.y);

                    mOwner.SetSize(size_new);
                }
            } else {
                mIsResizing = false;
            }
        }

        void Render(sf::RenderWindow& window) override {
            if (mOwner.GetDebugModeStatus()) {
                sf::Vector2f size_owner = mOwner.GetSize();
                sf::Vector2f pos_owner = mOwner.GetPosition();

                sf::RectangleShape handle({10.0f, 10.0f});
                handle.setPosition({pos_owner.x + size_owner.x - 10.0f, pos_owner.y + size_owner.y - 10.0f});
                handle.setFillColor(sf::Color(200, 200, 200));

                window.draw(handle);
            }
        }
    };
}