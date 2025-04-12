#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/DermaOption.hpp"
#include "Orbis/DermaEvent.hpp"
#include "Orbis/Enums.hpp"

namespace Orbis {
    class Derma {
    private:
        Derma* mParent = nullptr;
        std::vector<Derma*> mChildren;

        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        bool mWasPressed;
        bool mIsVisible;
        bool mIsRegistered;
        bool mIsInBounds;

        EventSystem mEventSystem;
        std::vector<std::shared_ptr<DermaOption>> mOptions;
        std::weak_ptr<Selectable> mOptionSelectable;
        std::weak_ptr<Movable> mOptionMovable;
        std::weak_ptr<Resizable> mOptionResizable;

        bool mIsDebugMode;

    public:
        Derma(DermaType, size_t id)
            : mID(id),
              mName("Unnamed"),
              mSize({0.0f, 0.0f}),
              mPosition({0.0f, 0.0f}),
              mZLevel(0),
              mWasPressed(false),
              mIsVisible(true),
              mIsRegistered(false),
              mIsInBounds(false),
              mIsDebugMode(false) {}

        template <typename T, typename... Args>
        std::shared_ptr<T> AddOption(Args&&... args) {
            auto option = std::make_shared<T>(*this, std::forward<Args>(args)...);
            auto notify = [this](DermaEventType type, const void* data) {
                DermaEvent event;

                event.mType = type;

                if (type == DermaEventType::Moved) {
                    event.mPosition = *static_cast<const sf::Vector2f*>(data);
                }

                mEventSystem.EmitEvent(event);
            };

            option->Initialize(mEventSystem, notify);
            mOptions.push_back(std::move(option));

            return option;
        }

        template <typename T>
        void RemoveOption() {
            if constexpr (std::is_same_v<T, Selectable>) {
                mOptionSelectable.reset();
            }

            if constexpr (std::is_same_v<T, Movable>) {
                mOptionMovable.reset();
            }

            if constexpr (std::is_same_v<T, Resizable>) {
                mOptionResizable.reset();
            }

            mOptions.erase(
                std::remove_if(mOptions.begin(), mOptions.end(), [](const auto& component) {
                    return dynamic_cast<T*>(component.get()) != nullptr;
                }),
                mOptions.end());
        }

        bool IsInBounds(sf::Vector2f cursor_position) const {
            return ((mPosition.x <= cursor_position.x) && (cursor_position.x <= mPosition.x + mSize.x)) &&
                   ((mPosition.y <= cursor_position.y) && (cursor_position.y <= mPosition.y + mSize.y));
        }

        bool IsSelected() const {
            auto selectable = mOptionSelectable.lock();

            if (selectable != nullptr)
                return selectable->GetSelectedStatus();

            return false;
        }

        bool IsMoved() const {
            auto movable = mOptionMovable.lock();

            if (movable != nullptr)
                return movable->GetMovedStatus();

            return false;
        }

        bool IsResized() const {
            auto resizable = mOptionResizable.lock();

            if (resizable != nullptr)
                return resizable->GetResizedStatus();

            return false;
        }

        bool HasFlag(DermaOptionFlag flags, DermaOptionFlag flag) const {
            return (flags & flag) == flag;
        }

        Derma* GetParent() const {
            return mParent;
        }

        const std::vector<Derma*> GetChildren() const {
            return mChildren;
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

        bool GetVisiblityStatus() const {
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
            for (const auto& component : mOptions) {
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
            mEventSystem.EmitEvent(event);

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

        Derma& SetRegistered(bool is_registered) {
            mIsRegistered = is_registered;

            return *this;
        }

        Derma& SetComponents(DermaOptionFlag flags) {
            bool has_selectable = HasFlag(flags, DermaOptionFlag::Selectable);

            SetSelectable(has_selectable);

            bool has_movable = HasFlag(flags, DermaOptionFlag::Movable);

            SetMovable(has_movable);

            bool has_resizable = HasFlag(flags, DermaOptionFlag::Resizable);

            SetResizable(has_resizable);

            return *this;
        }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (mOptionSelectable.expired() == true)) {
                mOptionSelectable = AddOption<Selectable>(mIsInBounds);
            } else if ((is_selectable == false) && (mOptionSelectable.expired() == false)) {
                RemoveOption<Selectable>();
            }
        }

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (mOptionMovable.expired() == true)) {
                mOptionMovable = AddOption<Movable>(mPosition, mIsInBounds);
            } else if ((is_movable == false) && (mOptionMovable.expired() == false)) {
                RemoveOption<Movable>();
            }
        }

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (mOptionResizable.expired() == true)) {
                mOptionResizable = AddOption<Resizable>(mPosition, mSize, mIsDebugMode);
            } else if ((is_resizable == false) && (mOptionResizable.expired() == false)) {
                RemoveOption<Resizable>();
            }
        }

        void ProcessControls(const Controls& controls) {
            sf::Vector2i pos_mouse = controls.GetMousePosition();
            sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};
            
            mIsInBounds = IsInBounds(pos_mouse_f);

            DermaEvent event_move;
            event_move.mType = DermaEventType::MouseMove;
            event_move.mPositionMouse = pos_mouse_f;
            event_move.mIsInBounds = IsInBounds(pos_mouse_f);
            mEventSystem.EmitEvent(event_move);

            bool is_pressed = controls.GetIsLMousePressed();

            if ((is_pressed == true) && (mWasPressed == false)) {
                DermaEvent event_lmouse_down;

                event_lmouse_down.mType = DermaEventType::MouseDown;
                event_lmouse_down.mPositionMouse = pos_mouse_f;
                event_lmouse_down.mIsInBounds = IsInBounds(pos_mouse_f);
                mEventSystem.EmitEvent(event_lmouse_down);
            } else if ((is_pressed == false) && (mWasPressed == true)) {
                DermaEvent event_lmouse_up;

                event_lmouse_up.mType = DermaEventType::MouseUp;
                event_lmouse_up.mPositionMouse = pos_mouse_f;
                event_lmouse_up.mIsInBounds = IsInBounds(pos_mouse_f);
                mEventSystem.EmitEvent(event_lmouse_up);
            }

            mWasPressed = is_pressed;
        }

        void Update(const Controls& controls) {
            if (mIsVisible == false)
                return;

            ProcessControls(controls);

            for (auto& component : mOptions) {
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

            for (auto& component : mOptions) {
                component->Render(window);
            }
        }
    };
}