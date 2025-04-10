#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "orbis/controls.hpp"
#include "orbis/enums.hpp"

namespace Orbis {
    class Derma;

    class DermaComponent {
    protected:
        Derma* mOwner;

    public:
        explicit DermaComponent(Derma& owner) : mOwner(&owner) {}
        virtual ~DermaComponent() = default;

        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow&) {};
    };

    class Selectable;
    class Movable;
    class Resizable;

    class Derma {
    private:
        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        bool mIsVisible;
        bool mIsDebugMode;
        bool mIsRegistered;

        std::vector<std::unique_ptr<DermaComponent>> mComponents;

        Selectable* mComponentSelectable = nullptr;
        Movable* mComponentMovable = nullptr;
        Resizable* mComponentResizable = nullptr;

    public:
        Derma(DermaType, size_t id)
            : mID(id),
              mName("Unnamed"),
              mSize({0.0f, 0.0f}),
              mPosition({0.0f, 0.0f}),
              mZLevel(0),
              mIsVisible(true),
              mIsDebugMode(false),
              mIsRegistered(false) {}

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
            if constexpr (std::is_same_v<T, Selectable>) {
                mComponentSelectable = nullptr;
            }

            if constexpr (std::is_same_v<T, Movable>) {
                mComponentMovable = nullptr;
            }

            if constexpr (std::is_same_v<T, Resizable>) {
                mComponentResizable = nullptr;
            }

            mComponents.erase(
                std::remove_if(mComponents.begin(), mComponents.end(), [](const auto& component) {
                    return dynamic_cast<T*>(component.get()) != nullptr;
                }),
                mComponents.end());
        }

        size_t GetID() const {
            return mID;
        }

        const std::string& GetName() const {
            return mName;
        }

        sf::Vector2f GetSize() const {
            return mSize;
        }

        sf::Vector2f GetPosition() const {
            return mPosition;
        }

        size_t GetZLevel() const {
            return mZLevel;
        }

        bool GetVisibleStatus() const {
            return mIsVisible;
        }

        bool GetDebugModeStatus() const {
            return mIsDebugMode;
        }

        bool GetRegisteredStatus() const {
            return mIsRegistered;
        }

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

        Derma& SetName(std::string name) {
            mName = std::move(name);

            return *this;
        }

        Derma& SetSize(sf::Vector2f size) {
            mSize = size;

            return *this;
        }

        Derma& SetPosition(sf::Vector2f position) {
            mPosition = position;

            return *this;
        }

        Derma& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;

            return *this;
        }

        Derma& SetVisible(bool is_visible) {
            mIsVisible = is_visible;

            return *this;
        }

        Derma& SetDebugMode(bool is_debugmode) {
            mIsDebugMode = is_debugmode;

            return *this;
        }

        void SetRegistered(bool is_registered) {
            mIsRegistered = is_registered;
        }

        Derma& SetComponents(DermaComponentFlag flags) {
            bool has_selectable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaComponentFlag::Selectable)) != 0;

            SetSelectable(has_selectable);

            bool has_movable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaComponentFlag::Movable)) != 0;

            SetMovable(has_movable);

            bool has_resizable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaComponentFlag::Resizable)) != 0;

            SetResizable(has_resizable);

            return *this;
        }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (mComponentSelectable == nullptr)) {
                mComponentSelectable = AddComponent<Selectable>();
            } else if ((is_selectable == false) && (mComponentSelectable != nullptr)) {
                RemoveComponent<Selectable>();
            }
        }

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (mComponentMovable == nullptr)) {
                mComponentMovable = AddComponent<Movable>();
            } else if ((is_movable == false) && (mComponentMovable != nullptr)) {
                RemoveComponent<Movable>();
            }
        }

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (mComponentResizable == nullptr)) {
                mComponentResizable = AddComponent<Resizable>();
            } else if ((is_resizable == false) && (mComponentResizable != nullptr)) {
                RemoveComponent<Resizable>();
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
        explicit Selectable(Derma& owner)
            : DermaComponent(owner), mIsSelected(false) {}

        bool GetSelectedStatus() const { return mIsSelected; }

        void Update(const Controls& controls) override {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            bool is_lmouse_pressed = controls.GetIsLMousePressed();

            if (is_lmouse_pressed == true) {
                if (mIsSelected == false && mOwner->ContainsCursor(pos_mouse_f)) {
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
        explicit Movable(Derma& owner)
            : DermaComponent(owner), mIsMoving(false), mOffsetMoving({0.0f, 0.0f}) {}

        bool GetMovingStatus() const { return mIsMoving; }

        void Update(const Controls& controls) override {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            bool is_lmouse_pressed = controls.GetIsLMousePressed();

            if (is_lmouse_pressed) {
                if (mIsMoving == false && mOwner->ContainsCursor(pos_mouse_f)) {
                    mIsMoving = true;
                    mOffsetMoving = pos_mouse_f - mOwner->GetPosition();
                }

                if (mIsMoving == true) {
                    mOwner->SetPosition(pos_mouse_f - mOffsetMoving);
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

            sf::Vector2f size_owner = mOwner->GetSize();
            sf::Vector2f pos_owner = mOwner->GetPosition();
            sf::Vector2f br_owner = pos_owner + size_owner;

            sf::Vector2f size_handle = {10.0f, 10.0f};
            sf::Vector2f pos_handle = {br_owner.x - size_handle.x, br_owner.y - size_handle.y};
            sf::Rect<float> handle(pos_handle, size_handle);
            bool is_in_handle = handle.contains(pos_mouse_f);

            if (is_rmouse_pressed == true) {
                if ((mIsResizing == false) && (is_in_handle == true)) {
                    mIsResizing = true;
                    mOffsetResizing = pos_mouse_f - br_owner;
                }

                if (mIsResizing == true) {
                    sf::Vector2f size_new = (pos_mouse_f - mOffsetResizing) - pos_owner;

                    size_new.x = std::max(size_new.x, mMinimumSize.x);
                    size_new.y = std::max(size_new.y, mMinimumSize.y);

                    mOwner->SetSize(size_new);
                }
            } else {
                mIsResizing = false;
            }
        }

        void Render(sf::RenderWindow& window) override {
            if (mOwner->GetDebugModeStatus() == true) {
                sf::Vector2f size_owner = mOwner->GetSize();
                sf::Vector2f pos_owner = mOwner->GetPosition();

                sf::RectangleShape handle({10.0f, 10.0f});
                handle.setPosition({pos_owner.x + size_owner.x - 10.0f, pos_owner.y + size_owner.y - 10.0f});
                handle.setFillColor(sf::Color(200, 200, 200));

                window.draw(handle);
            }
        }
    };
}