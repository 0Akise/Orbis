#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Derma/DermaDrawings.hpp"
#include "Orbis/Derma/DermaEnums.hpp"
#include "Orbis/Derma/DermaOptions.hpp"

namespace Orbis {
    template <typename Derived>
    class Derma : public DermaInterface, public std::enable_shared_from_this<Derma<Derived>> {
    private:
        std::weak_ptr<DermaInterface> mParent;
        std::vector<std::shared_ptr<DermaInterface>> mChildren;

        size_t mID;
        std::string mName;
        sf::Vector2f mSize;
        sf::Vector2f mPosition;
        size_t mZLevel;

        std::multimap<size_t, std::shared_ptr<DermaDrawings>> mDrawings;

        Controls mControlsPrevious;

        bool mIsVisible;
        bool mIsRegistered;
        bool mIsInBounds;

        std::vector<std::shared_ptr<DermaOption>> mOptions;
        std::weak_ptr<Selectable> mOptionSelectable;
        std::weak_ptr<Movable> mOptionMovable;
        std::weak_ptr<Resizable> mOptionResizable;

        bool mIsDebugMode;

    protected:
        DermaEventSystem mEventSystem;

        Derived& self() {
            return *static_cast<Derived*>(this);
        }

        const Derived& self() const {
            return *static_cast<const Derived*>(this);
        }

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

        static std::shared_ptr<DermaInterface> Create(DType type, size_t id) {
            return std::make_shared<DermaInterface>(type, id);
        }

        void SetParent(std::shared_ptr<DermaInterface> parent) override {
            mParent = parent;
        }

        void AddChild(std::shared_ptr<DermaInterface> child) override {
            if (auto parent = child->GetParent()) {
                parent->RemoveChild(child);
            }

            child->SetParent(this->shared_from_this());
            mChildren.push_back(child);

            DEvent event_parent_changed;
            event_parent_changed.mType = DEventType::ParentChanged;
            mEventSystem.EmitEvent(event_parent_changed);

            DEvent event_child_added;
            event_child_added.mType = DEventType::ChildAdded;
            mEventSystem.EmitEvent(event_child_added);
        }

        void RemoveChild(const std::shared_ptr<DermaInterface>& child) override {
            auto iter = std::find(mChildren.begin(), mChildren.end(), child);

            if (iter != mChildren.end()) {
                (*iter)->SetParent(nullptr);
                mChildren.erase(iter);

                DEvent event_child_removed;
                event_child_removed.mType = DEventType::ChildRemoved;
                mEventSystem.EmitEvent(event_child_removed);
            }
        }

        std::shared_ptr<DermaInterface> GetParent() const override {
            return mParent.lock();
        }

        const std::vector<std::shared_ptr<DermaInterface>>& GetChildren() const override {
            return mChildren;
        }

        template <typename T, typename... Args>
        std::shared_ptr<T> AddOption(Args&&... args) {
            auto option = std::make_shared<T>(this->shared_from_this(), std::forward<Args>(args)...);
            auto notify = [this](DEventType type, const void* data) {
                DEvent event;

                event.mType = type;

                if (type == DEventType::Moved) {
                    const sf::Vector2f* position = static_cast<const sf::Vector2f*>(data);
                    event.mPosition = *position;
                    this->mPosition = *position;
                } else if (type == DEventType::Resized) {
                    const sf::Vector2f* size = static_cast<const sf::Vector2f*>(data);
                    event.mSize = *size;
                    this->mSize = *size;
                }

                this->mEventSystem.EmitEvent(event);
            };

            option->Initialize(this->mEventSystem, notify);
            this->mOptions.push_back(std::move(option));

            return option;
        }

        template <typename T>
        void RemoveOption() {
            if constexpr (std::is_same_v<T, Selectable>) {
                this->mOptionSelectable.reset();
            }

            if constexpr (std::is_same_v<T, Movable>) {
                this->mOptionMovable.reset();
            }

            if constexpr (std::is_same_v<T, Resizable>) {
                this->mOptionResizable.reset();
            }

            this->mOptions.erase(
                std::remove_if(this->mOptions.begin(), this->mOptions.end(), [](const auto& option) {
                    return dynamic_cast<T*>(option.get()) != nullptr;
                }),
                this->mOptions.end());
        }

        template <typename T>
        T* GetOption() const {
            for (const auto& option : this->mOptions) {
                auto* casted = dynamic_cast<T*>(option.get());

                if (casted != nullptr) {
                    return casted;
                }
            }

            return nullptr;
        }

        Derived& SetOptions(DOption flags) {
            bool has_selectable = HasOptionFlag(flags, DOption::Selectable);
            SetSelectable(has_selectable);

            bool has_movable = HasOptionFlag(flags, DOption::Movable);
            SetMovable(has_movable);

            bool has_resizable = HasOptionFlag(flags, DOption::Resizable);
            SetResizable(has_resizable);

            return self();
        }

        void SetSelectable(bool is_selectable) {
            if ((is_selectable == true) && (this->mOptionSelectable.expired() == true)) {
                this->mOptionSelectable = AddOption<Selectable>();
            } else if ((is_selectable == false) && (this->mOptionSelectable.expired() == false)) {
                RemoveOption<Selectable>();
            }
        }

        void SetMovable(bool is_movable) {
            if ((is_movable == true) && (this->mOptionMovable.expired() == true)) {
                this->mOptionMovable = AddOption<Movable>(this->mPosition);
            } else if ((is_movable == false) && (this->mOptionMovable.expired() == false)) {
                RemoveOption<Movable>();
            }
        }

        void SetResizable(bool is_resizable) {
            if ((is_resizable == true) && (this->mOptionResizable.expired() == true)) {
                this->mOptionResizable = AddOption<Resizable>(this->mSize);
            } else if ((is_resizable == false) && (this->mOptionResizable.expired() == false)) {
                RemoveOption<Resizable>();
            }
        }

        bool IsSelected() const {
            auto selectable = this->mOptionSelectable.lock();

            if (selectable != nullptr)
                return selectable->GetSelectedStatus();

            return false;
        }

        bool IsMoved() const {
            auto movable = this->mOptionMovable.lock();

            if (movable != nullptr)
                return movable->GetMovedStatus();

            return false;
        }

        bool IsResized() const {
            auto resizable = this->mOptionResizable.lock();

            if (resizable != nullptr)
                return resizable->GetResizedStatus();

            return false;
        }

        bool HasOptionFlag(DOption flags, DOption flag) const {
            return (flags & flag) == flag;
        }

        bool IsInBounds(sf::Vector2f cursor_position) const {
            sf::Vector2f pos_global = GetPositionGlobal();

            return ((pos_global.x <= cursor_position.x) && (cursor_position.x <= pos_global.x + mSize.x)) &&
                   ((pos_global.y <= cursor_position.y) && (cursor_position.y <= pos_global.y + mSize.y));
        }

        template <typename T = Derived>
        std::shared_ptr<T> GetSharedFromThis() {
            return std::static_pointer_cast<T>(this->shared_from_this());
        }

        size_t GetID() const override {
            return mID;
        }

        const std::string& GetName() const override {
            return mName;
        }

        sf::Vector2f GetSize() const {
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

        bool GetRegisteredStatus() const {
            return mIsRegistered;
        }

        bool GetDebugModeStatus() const {
            return mIsDebugMode;
        }

        Derived& SetName(std::string name) {
            mName = std::move(name);
            return self();
        }

        Derived& SetSize(sf::Vector2f size) {
            mSize = size;

            DEvent event_resized;
            event_resized.mType = DEventType::Resized;
            event_resized.mSize = size;
            mEventSystem.EmitEvent(event_resized);

            return self();
        }

        Derived& SetPosition(sf::Vector2f position) {
            mPosition = position;

            DEvent event_moved;
            event_moved.mType = DEventType::Moved;
            event_moved.mPosition = position;
            mEventSystem.EmitEvent(event_moved);

            return self();
        }

        Derived& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;
            return self();
        }

        Derived& SetVisible(bool is_visible) {
            mIsVisible = is_visible;
            return self();
        }

        Derived& SetRegistered(bool is_registered) {
            mIsRegistered = is_registered;
            return self();
        }

        Derived& SetDebugMode(bool is_debugmode) {
            mIsDebugMode = is_debugmode;
            return self();
        }

        void Update(const Controls& controls) override {
            if (this->mIsVisible == false)
                return;

            ProcessControls(controls);

            for (auto& child : mChildren) {
                child->Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) override {
            if (this->mIsVisible == false)
                return;

            sf::Vector2f pos_global = this->GetPositionGlobal();

            for (const auto& [zlevel, drawing] : this->mDrawings) {
                ProcessDrawings(window, drawing, pos_global);
            }

            if (this->mIsDebugMode == true) {
                ProcessDebugMode(window, pos_global);
            }

            for (auto& child : mChildren) {
                child->Render(window);
            }
        }

        void ProcessControls(const Controls& controls) {
            this->mIsInBounds = this->IsInBounds(controls.mMouse.mPosition);

            DEvent event_base;
            event_base.mPosition = this->GetPositionGlobal();
            event_base.mSize = this->mSize;
            event_base.mZLevel = this->mZLevel;
            event_base.mIsInBounds = this->mIsInBounds;
            event_base.mControls.mMouse.mPosition = controls.mMouse.mPosition;
            event_base.mControls.mMouse.mLPress = controls.mMouse.mLPress;
            event_base.mControls.mMouse.mRPress = controls.mMouse.mRPress;
            event_base.mControls.mMouse.mWPress = controls.mMouse.mWPress;
            event_base.mIsVisible = this->mIsVisible;

            DEvent event_move = event_base;
            event_move.mType = DEventType::MouseMove;
            this->mEventSystem.EmitEvent(event_move);

            if ((event_base.mControls.mMouse.mLPress == true) && (this->mControlsPrevious.mMouse.mLPress == false)) {
                DEvent event_mouse_down = event_base;

                event_mouse_down.mType = DEventType::MouseLDown;
                this->mEventSystem.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mLPress == false) && (this->mControlsPrevious.mMouse.mLPress == true)) {
                DEvent event_mouse_up = event_base;

                event_mouse_up.mType = DEventType::MouseLUp;
                this->mEventSystem.EmitEvent(event_mouse_up);
            }

            if ((event_base.mControls.mMouse.mRPress == true) && (this->mControlsPrevious.mMouse.mRPress == false)) {
                DEvent event_mouse_down = event_base;

                event_mouse_down.mType = DEventType::MouseRDown;
                this->mEventSystem.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mRPress == false) && (this->mControlsPrevious.mMouse.mRPress == true)) {
                DEvent event_mouse_up = event_base;

                event_mouse_up.mType = DEventType::MouseRUp;
                this->mEventSystem.EmitEvent(event_mouse_up);
            }

            this->mControlsPrevious.mMouse.mLPress = event_base.mControls.mMouse.mLPress;
            this->mControlsPrevious.mMouse.mRPress = event_base.mControls.mMouse.mRPress;
        }

        void ProcessDrawings(sf::RenderWindow& window, const std::shared_ptr<DermaDrawings>& drawing, const sf::Vector2f& pos_global) {
            sf::Vector2f pos_drawing = pos_global + drawing->mPosition;

            switch (drawing->mType) {
                case DDrawingsType::Rect: {
                    auto drawing_rect = std::static_pointer_cast<DrawingsRect>(drawing);

                    if (drawing_rect->mIsRounded == true) {
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
                    sf::Text text(drawing_text->mFont, drawing_text->mText, drawing_text->mFontSize);

                    text.setPosition(pos_drawing);
                    text.setFillColor(drawing_text->mFillColor);
                    window.draw(text);

                    break;
                }

                case DDrawingsType::Texture: {
                    auto drawing_texture = std::static_pointer_cast<DrawingsTexture>(drawing);

                    sf::RectangleShape shape(drawing_texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(drawing_texture->mFillColor);

                    if (drawing_texture->mTextureSmoothing == true) {
                        drawing_texture->mTexture.setSmooth(true);
                    }

                    shape.setTexture(&drawing_texture->mTexture);
                    window.draw(shape);

                    break;
                }

                case DDrawingsType::Sprite: {
                    break;
                }
            }
        }

        void ProcessDebugMode(sf::RenderWindow& window, const sf::Vector2f& pos_global) {
            sf::RectangleShape rect(this->mSize);

            rect.setPosition(pos_global);
            rect.setFillColor(sf::Color(255, 255, 255, 128));
            rect.setOutlineColor(sf::Color::White);
            rect.setOutlineThickness(1.0f);

            window.draw(rect);
        }

        Derived& DrawRect(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f) {
            auto drawing = std::make_shared<DrawingsRect>();

            drawing->mType = DDrawingsType::Rect;
            drawing->mID = "";
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mIsOutlined = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor = outline_color;
            drawing->mIsRounded = is_rounded;
            drawing->mRoundingRadius = rounding_radius;
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawRectDynamic(
            const std::string& id,
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f) {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingsRect>();

            drawing->mType = DDrawingsType::Rect;
            drawing->mID = id;
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mIsOutlined = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor = outline_color;
            drawing->mIsRounded = is_rounded;
            drawing->mRoundingRadius = rounding_radius;
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawText(
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            std::string text = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType = DDrawingsType::Text;
            drawing->mID = "";
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTextDynamic(
            const std::string& id,
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            std::string text = "") {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType = DDrawingsType::Text;
            drawing->mID = id;
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTexture(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            sf::Texture texture,
            bool smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType = DDrawingsType::Texture;
            drawing->mID = "";
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mTexture = texture;
            drawing->mTextureSmoothing = smoothing_enabled;
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTextureDynamic(
            const std::string& id,
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            sf::Texture texture,
            bool smoothing_enabled = true) {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType = DDrawingsType::Texture;
            drawing->mID = id;
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mTexture = texture;
            drawing->mTextureSmoothing = smoothing_enabled;
            this->mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& ClearDrawing(const std::string& id) {
            if (id.empty() == true) {
                return self();
            }

            std::vector<std::multimap<size_t, std::shared_ptr<DermaDrawings>>::iterator> to_erase;

            for (auto iter = this->mDrawings.begin(); iter != this->mDrawings.end(); ++iter) {
                if (iter->second->mID == id) {
                    to_erase.push_back(iter);
                }
            }

            for (auto& iter : to_erase) {
                this->mDrawings.erase(iter);
            }

            return self();
        }

        Derived& ClearDrawingsAll() {
            this->mDrawings.clear();

            return self();
        }
    };
}