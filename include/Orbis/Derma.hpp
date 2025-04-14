#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/Data.hpp"
#include "Orbis/DermaEvent.hpp"
#include "Orbis/DermaInterface.hpp"
#include "Orbis/DermaOption.hpp"

namespace Orbis {
    class Derma : public DermaInterface {
    private:
        Derma* mParent = nullptr;
        std::vector<Derma*> mChildren;

        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        EventSystem mEventSystem;

        bool mWasLMousePressed;
        bool mWasRMousePressed;
        bool mIsVisible;
        bool mIsRegistered;
        bool mIsInBounds;

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
              mWasLMousePressed(false),
              mWasRMousePressed(false),
              mIsVisible(true),
              mIsRegistered(false),
              mIsInBounds(false),
              mIsDebugMode(false) {}

        void AddChild(Derma& child) {
            if (child.mParent != nullptr)
                child.mParent->RemoveChild(child);

            child.mParent = this;
            mChildren.push_back(&child);

            DermaEvent event_parent_changed;
            event_parent_changed.mType = DermaEventType::ParentChanged;
            mEventSystem.EmitEvent(event_parent_changed);

            DermaEvent event_child_added;
            event_child_added.mType = DermaEventType::ChildAdded;
            mEventSystem.EmitEvent(event_child_added);
        }

        void RemoveChild(Derma& child) {
            auto iter = std::find(mChildren.begin(), mChildren.end(), &child);

            if (iter != mChildren.end()) {
                (*iter)->mParent = nullptr;

                mChildren.erase(iter);

                DermaEvent event_child_removed;
                event_child_removed.mType = DermaEventType::ChildRemoved;
                mEventSystem.EmitEvent(event_child_removed);
            }
        }

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
                std::remove_if(mOptions.begin(), mOptions.end(), [](const auto& option) {
                    return dynamic_cast<T*>(option.get()) != nullptr;
                }),
                mOptions.end());
        }

        Derma* GetParent() const {
            return mParent;
        }

        const std::vector<Derma*>& GetChildren() const {
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

        sf::Vector2f GetPositionLocal() const {
            return mPosition;
        }

        sf::Vector2f GetPositionGlobal() const override {
            if (mParent == nullptr)
                return mPosition;

            return mParent->GetPositionGlobal() + mPosition;
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
        T* GetOption() const {
            for (const auto& option : mOptions) {
                auto* casted = dynamic_cast<T*>(option.get());

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

            DermaEvent event_moved;

            event_moved.mType = DermaEventType::Moved;
            event_moved.mPosition = position;
            mEventSystem.EmitEvent(event_moved);

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

        Derma& SetOptions(DermaOptionFlag flags) {
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

        bool IsInBounds(sf::Vector2f cursor_position) const {
            sf::Vector2f pos_global = GetPositionGlobal();

            return ((pos_global.x <= cursor_position.x) && (cursor_position.x <= pos_global.x + mSize.x)) &&
                   ((pos_global.y <= cursor_position.y) && (cursor_position.y <= pos_global.y + mSize.y));
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

        void Update(const Controls& controls);
        void Render(sf::RenderWindow& window);
        void ProcessControls(const Controls& controls);
    };
}

namespace Orbis {
    void Derma::Update(const Controls& controls) {
        if (mIsVisible == false)
            return;

        ProcessControls(controls);

        for (auto& option : mOptions) {
            option->Update(controls);
        }
    }

    void Derma::Render(sf::RenderWindow& window) {
        if (mIsVisible == false)
            return;

        sf::Vector2f pos_global = GetPositionGlobal();

        if (mIsDebugMode == true) {
            sf::RectangleShape rect(mSize);
            rect.setPosition(pos_global);
            rect.setFillColor(sf::Color(255, 255, 255, 128));
            rect.setOutlineColor(sf::Color::White);
            rect.setOutlineThickness(1.0f);

            window.draw(rect);
        }

        for (auto& option : mOptions) {
            option->Render(window);
        }
    }

    void Derma::ProcessControls(const Controls& controls) {
        sf::Vector2i pos_mouse = controls.GetMousePosition();
        sf::Vector2f pos_mouse_f = {static_cast<float>(pos_mouse.x), static_cast<float>(pos_mouse.y)};

        mIsInBounds = IsInBounds(pos_mouse_f);

        DermaEvent event_base;

        event_base.mPosition = GetPositionGlobal();
        event_base.mSize = mSize;
        event_base.mPositionMouse = pos_mouse_f;
        event_base.mZLevel = mZLevel;
        event_base.mIsInBounds = mIsInBounds;
        event_base.mIsLMousePressed = controls.GetIsLMousePressed();
        event_base.mIsRMousePressed = controls.GetIsRMousePressed();
        event_base.mIsWMousePressed = controls.GetIsWMousePressed();
        event_base.mIsVisible = mIsVisible;

        DermaEvent event_move = event_base;

        event_move.mType = DermaEventType::MouseMove;
        mEventSystem.EmitEvent(event_move);

        if ((event_base.mIsLMousePressed == true) && (mWasLMousePressed == false)) {
            DermaEvent event_mouse_down = event_base;

            event_mouse_down.mType = DermaEventType::MouseDown;
            mEventSystem.EmitEvent(event_mouse_down);

            event_mouse_down.mType = DermaEventType::MouseLDown;
            mEventSystem.EmitEvent(event_mouse_down);
        } else if ((event_base.mIsLMousePressed == false) && (mWasLMousePressed == true)) {
            DermaEvent event_mouse_up = event_base;

            event_mouse_up.mType = DermaEventType::MouseUp;
            mEventSystem.EmitEvent(event_mouse_up);

            event_mouse_up.mType = DermaEventType::MouseLUp;
            mEventSystem.EmitEvent(event_mouse_up);
        }

        if ((event_base.mIsRMousePressed == true) && (mWasRMousePressed == false)) {
            DermaEvent event_mouse_down = event_base;

            event_mouse_down.mType = DermaEventType::MouseDown;
            mEventSystem.EmitEvent(event_mouse_down);

            event_mouse_down.mType = DermaEventType::MouseRDown;
            mEventSystem.EmitEvent(event_mouse_down);
        } else if ((event_base.mIsRMousePressed == false) && (mWasRMousePressed == true)) {
            DermaEvent event_mouse_up = event_base;

            event_mouse_up.mType = DermaEventType::MouseUp;
            mEventSystem.EmitEvent(event_mouse_up);

            event_mouse_up.mType = DermaEventType::MouseRUp;
            mEventSystem.EmitEvent(event_mouse_up);
        }

        mWasLMousePressed = event_base.mIsLMousePressed;
        mWasRMousePressed = event_base.mIsRMousePressed;
    }
}