#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/DermaComponent.hpp"
#include "Orbis/DermaEvent.hpp"
#include "Orbis/Enums.hpp"

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
        bool mIsRegistered;

        EventSystem mEvent;
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

        template <typename T, typename... Args>
        T* AddComponent(Args&&... args) {
            auto component = std::make_unique<T>(*this, std::forward<Args>(args)...);
            T* raw_pointer = component.get();

            component->Initialize(mEvent);
            mComponents.push_back(std::move(component));

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

        bool IsInBounds(sf::Vector2f cursor_position) const {
            return ((mPosition.x <= cursor_position.x) && (cursor_position.x <= mPosition.x + mSize.x)) &&
                   ((mPosition.y <= cursor_position.y) && (cursor_position.y <= mPosition.y + mSize.y));
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

            DermaEvent event;

            event.mType = DermaEventType::Moved;
            event.mPosition = position;
            mEvent.EmitEvent(event);

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

        Derma& SetComponents(DermaOptionFlag flags) {
            bool has_selectable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaOptionFlag::Selectable)) != 0;

            SetSelectable(has_selectable);

            bool has_movable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaOptionFlag::Movable)) != 0;

            SetMovable(has_movable);

            bool has_resizable = (static_cast<uint32_t>(flags) & static_cast<uint32_t>(DermaOptionFlag::Resizable)) != 0;

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

        void ProcessControls(const Controls& controls) {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            DermaEvent event_move;

            event_move.mType = DermaEventType::MouseMove;
            event_move.mPositionMouse = pos_mouse_f;
            event_move.mIsInBounds = IsInBounds(pos_mouse_f);

            mEvent.EmitEvent(event_move);

            static bool was_pressed = false;
            bool is_pressed = controls.GetIsLMousePressed();

            if ((is_pressed == true) && (was_pressed == false)) {
                DermaEvent event_lmouse_down;

                event_lmouse_down.mType = DermaEventType::MouseDown;
                event_lmouse_down.mPositionMouse = pos_mouse_f;
                event_lmouse_down.mIsInBounds = IsInBounds(pos_mouse_f);
                mEvent.EmitEvent(event_lmouse_down);
            } else if ((is_pressed == false) && (was_pressed == true)) {
                DermaEvent event_lmouse_up;

                event_lmouse_up.mType = DermaEventType::MouseUp;
                event_lmouse_up.mPositionMouse = pos_mouse_f;
                event_lmouse_up.mIsInBounds = IsInBounds(pos_mouse_f);
                mEvent.EmitEvent(event_lmouse_up);
            }

            was_pressed = is_pressed;
        }

        void Update(const Controls& controls) {
            if (mIsVisible == false)
                return;

            ProcessControls(controls);

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
}