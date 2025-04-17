#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/Data.hpp"
#include "Orbis/DermaDrawings.hpp"
#include "Orbis/DermaEvent.hpp"
#include "Orbis/DermaInterface.hpp"
#include "Orbis/DermaOption.hpp"

namespace Orbis {
    class Derma : public DermaInterface, public std::enable_shared_from_this<Derma> {
    private:
        std::weak_ptr<Derma> mParent;
        std::vector<std::shared_ptr<Derma>> mChildren;

        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        std::multimap<size_t, std::shared_ptr<DermaDrawings>> mDrawings;

        DermaEventSystem mEventSystem;
        MouseState mMousePrevious;

        bool mIsVisible;
        bool mIsRegistered;
        bool mIsInBounds;

        std::vector<std::shared_ptr<DermaOption>> mOptions;
        std::weak_ptr<Selectable> mOptionSelectable;
        std::weak_ptr<Movable> mOptionMovable;
        std::weak_ptr<Resizable> mOptionResizable;

        bool mIsDebugMode;

    public:
        Derma(DType, size_t id)
            : mID(id),
              mName("Unnamed"),
              mSize({0.0f, 0.0f}),
              mPosition({0.0f, 0.0f}),
              mZLevel(0),
              mIsVisible(true),
              mIsRegistered(false),
              mIsInBounds(false),
              mIsDebugMode(false) {}

        static std::shared_ptr<Derma> Create(DType type, size_t id) {
            return std::make_shared<Derma>(type, id);
        }

        void AddChild(std::shared_ptr<Derma> child) {
            if (auto parent = child->mParent.lock()) {
                parent->RemoveChild(child);
            }

            child->mParent = shared_from_this();
            mChildren.push_back(std::move(child));

            DEvent event_parent_changed;
            event_parent_changed.mType = DEventType::ParentChanged;
            mEventSystem.EmitEvent(event_parent_changed);

            DEvent event_child_added;
            event_child_added.mType = DEventType::ChildAdded;
            mEventSystem.EmitEvent(event_child_added);
        }

        void RemoveChild(const std::shared_ptr<Derma>& child) {
            auto iter = std::find(mChildren.begin(), mChildren.end(), child);

            if (iter != mChildren.end()) {
                (*iter)->mParent.reset();
                mChildren.erase(iter);

                DEvent event_child_removed;
                event_child_removed.mType = DEventType::ChildRemoved;
                mEventSystem.EmitEvent(event_child_removed);
            }
        }

        template <typename T, typename... Args>
        std::shared_ptr<T> AddOption(Args&&... args) {
            auto option = std::make_shared<T>(shared_from_this(), std::forward<Args>(args)...);
            auto notify = [this](DEventType type, const void* data) {
                DEvent event;

                event.mType = type;

                if (type == DEventType::Moved) {
                    const sf::Vector2f* position = static_cast<const sf::Vector2f*>(data);
                    event.mPosition = *position;
                    mPosition = *position;
                } else if (type == DEventType::Resized) {
                    const sf::Vector2f* size = static_cast<const sf::Vector2f*>(data);
                    event.mSize = *size;
                    mSize = *size;
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

        std::shared_ptr<Derma> GetParent() const {
            return mParent.lock();
        }

        const std::vector<std::shared_ptr<Derma>>& GetChildren() const {
            return mChildren;
        }

        size_t GetID() const {
            return mID;
        }

        const std::string& GetName() const {
            return mName;
        }

        sf::Vector2f GetSize() const override {
            return mSize;
        }

        sf::Vector2f GetPositionLocal() const override {
            return mPosition;
        }

        sf::Vector2f GetPositionGlobal() const override {
            auto parent = mParent.lock();

            if (parent == nullptr)
                return mPosition;

            return parent->GetPositionGlobal() + mPosition;
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

            DEvent event_resized;

            event_resized.mType = DEventType::Resized;
            event_resized.mSize = size;
            mEventSystem.EmitEvent(event_resized);

            return *this;
        }

        Derma& SetPosition(sf::Vector2f position) {
            mPosition = position;

            DEvent event_moved;

            event_moved.mType = DEventType::Moved;
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

        Derma& SetOptions(DOptionFlag flags) {
            bool has_selectable = HasOptionFlag(flags, DOptionFlag::Selectable);

            SetSelectable(has_selectable);

            bool has_movable = HasOptionFlag(flags, DOptionFlag::Movable);

            SetMovable(has_movable);

            bool has_resizable = HasOptionFlag(flags, DOptionFlag::Resizable);

            SetResizable(has_resizable);

            return *this;
        }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (mOptionSelectable.expired() == true)) {
                mOptionSelectable = AddOption<Selectable>();
            } else if ((is_selectable == false) && (mOptionSelectable.expired() == false)) {
                RemoveOption<Selectable>();
            }
        }

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (mOptionMovable.expired() == true)) {
                mOptionMovable = AddOption<Movable>(mPosition);
            } else if ((is_movable == false) && (mOptionMovable.expired() == false)) {
                RemoveOption<Movable>();
            }
        }

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (mOptionResizable.expired() == true)) {
                mOptionResizable = AddOption<Resizable>(mSize, mIsDebugMode);
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

        bool HasOptionFlag(DOptionFlag flags, DOptionFlag flag) const {
            return (flags & flag) == flag;
        }

        void Update(const Controls& controls);
        void Render(sf::RenderWindow& window);
        void ProcessControls(const Controls& controls);
        void ProcessDrawings(sf::RenderWindow& window, const std::shared_ptr<DermaDrawings>& drawing, const sf::Vector2f& pos_global);
        void ProcessDebugMode(sf::RenderWindow& window, const sf::Vector2f& pos_global);

        Derma& DrawRect(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f);

        Derma& DrawText(
            const std::string& font_path,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            std::string text = "");
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

        for (const auto& [zlevel, drawing] : mDrawings) {
            ProcessDrawings(window, drawing, pos_global);
        }

        if (mIsDebugMode == true) {
            ProcessDebugMode(window, pos_global);
        }

        for (auto& option : mOptions) {
            option->Render(window);
        }
    }

    void Derma::ProcessControls(const Controls& controls) {
        mIsInBounds = IsInBounds(controls.GetMousePosition());

        DEvent event_base;

        event_base.mPosition = GetPositionGlobal();
        event_base.mSize = mSize;
        event_base.mZLevel = mZLevel;
        event_base.mIsInBounds = mIsInBounds;
        event_base.mMouseState.mPosition = controls.GetMousePosition();
        event_base.mMouseState.mLPress = controls.GetIsLMousePressed();
        event_base.mMouseState.mRPress = controls.GetIsRMousePressed();
        event_base.mMouseState.mWPress = controls.GetIsWMousePressed();
        event_base.mIsVisible = mIsVisible;

        DEvent event_move = event_base;

        event_move.mType = DEventType::MouseMove;
        mEventSystem.EmitEvent(event_move);

        if ((event_base.mMouseState.mLPress == true) && (mMousePrevious.mLPress == false)) {
            DEvent event_mouse_down = event_base;

            event_mouse_down.mType = DEventType::MouseLDown;
            mEventSystem.EmitEvent(event_mouse_down);
        } else if ((event_base.mMouseState.mLPress == false) && (mMousePrevious.mLPress == true)) {
            DEvent event_mouse_up = event_base;

            event_mouse_up.mType = DEventType::MouseLUp;
            mEventSystem.EmitEvent(event_mouse_up);
        }

        if ((event_base.mMouseState.mRPress == true) && (mMousePrevious.mRPress == false)) {
            DEvent event_mouse_down = event_base;

            event_mouse_down.mType = DEventType::MouseRDown;
            mEventSystem.EmitEvent(event_mouse_down);
        } else if ((event_base.mMouseState.mRPress == false) && (mMousePrevious.mRPress == true)) {
            DEvent event_mouse_up = event_base;

            event_mouse_up.mType = DEventType::MouseRUp;
            mEventSystem.EmitEvent(event_mouse_up);
        }

        mMousePrevious.mLPress = event_base.mMouseState.mLPress;
        mMousePrevious.mRPress = event_base.mMouseState.mRPress;
    }

    void Derma::ProcessDrawings(sf::RenderWindow& window, const std::shared_ptr<DermaDrawings>& drawing, const sf::Vector2f& pos_global) {
        sf::Vector2f pos_drawing = pos_global + drawing->mPosition;

        switch (drawing->mType) {
            case DDrawingsType::Rect: {
                auto drawing_rect = std::static_pointer_cast<DrawingsRect>(drawing);

                if (drawing_rect->mIsRounded == true) {
                    // SFMLExt/sf::RoundedRectangleShape is not implemented yet
                    sf::RectangleShape shape(drawing_rect->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(drawing_rect->mFillColor);

                    if (drawing_rect->mIsOutlined == true) {
                        shape.setOutlineThickness(drawing_rect->mOutlineThickness);
                        shape.setOutlineColor(drawing_rect->mOutlineColor);
                    }

                    window.draw(shape);

                    break;
                } else {
                    sf::RectangleShape shape(drawing_rect->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(drawing_rect->mFillColor);

                    if (drawing_rect->mIsOutlined == true) {
                        shape.setOutlineThickness(drawing_rect->mOutlineThickness);
                        shape.setOutlineColor(drawing_rect->mOutlineColor);
                    }

                    window.draw(shape);

                    break;
                }
            }

            case DDrawingsType::Text: {
                auto drawing_text = std::static_pointer_cast<DrawingsText>(drawing);
                sf::Font font(drawing_text->mFontPath);
                sf::Text text(font, drawing_text->mText, drawing_text->mFontSize);

                text.setPosition(pos_drawing);
                text.setFillColor(drawing_text->mFillColor);

                window.draw(text);

                break;
            }

            case DDrawingsType::Image: {
                // Do stuff for image drawing
                break;
            }
        }
    }

    void Derma::ProcessDebugMode(sf::RenderWindow& window, const sf::Vector2f& pos_global) {
        sf::RectangleShape rect(mSize);

        rect.setPosition(pos_global);
        rect.setFillColor(sf::Color(255, 255, 255, 128));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(1.0f);

        window.draw(rect);
    }

    Derma& Derma::DrawRect(
        sf::Vector2f size,
        sf::Vector2f position,
        size_t zlevel,
        sf::Color fill_color,
        bool is_outlined,
        float outline_thickness,
        sf::Color outline_color,
        bool is_rounded,
        float rounding_radius) {
        auto drawing = std::make_shared<DrawingsRect>();

        drawing->mType = DDrawingsType::Rect;
        drawing->mSize = size;
        drawing->mPosition = position;
        drawing->mZLevel = zlevel;
        drawing->mFillColor = fill_color;
        drawing->mIsOutlined = is_outlined;
        drawing->mOutlineThickness = outline_thickness;
        drawing->mOutlineColor = outline_color;
        drawing->mIsRounded = is_rounded;
        drawing->mRoundingRadius = rounding_radius;
        mDrawings.emplace(zlevel, drawing);

        return *this;
    }

    Derma& Derma::DrawText(
        const std::string& font_path,
        size_t font_size,
        sf::Vector2f position,
        size_t zlevel,
        sf::Color fill_color,
        std::string text) {
        auto drawing = std::make_shared<DrawingsText>();

        drawing->mType = DDrawingsType::Text;
        drawing->mFontPath = font_path;
        drawing->mPosition = position;
        drawing->mZLevel = zlevel;
        drawing->mFillColor = fill_color;
        drawing->mFontSize = font_size;
        drawing->mText = std::move(text);
        mDrawings.emplace(zlevel, drawing);

        return *this;
    }
}