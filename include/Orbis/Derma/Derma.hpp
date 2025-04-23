#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Derma/DermaBase.hpp"
#include "Orbis/Derma/DermaCommons.hpp"
#include "Orbis/Derma/DermaDrawings.hpp"
#include "Orbis/Derma/DermaEvent.hpp"

namespace Orbis {
    template <typename Derived>
    class Derma : public DermaBase {
    private:
        DOption mOptionFlag = DOption::None;
        bool mIsSelected = false;
        bool mIsMoving = false;
        bool mIsResizing = false;
        sf::Vector2f mOffsetOption;

        size_t mID;
        std::string mName = "Unnamed";
        sf::Vector2f mSize = {0, 0};
        sf::Vector2f mPosition = {0, 0};
        size_t mZLevel = 0;

        std::multimap<size_t, std::shared_ptr<DermaDrawings>> mDrawings;

        Controls mControlsPrevious;
        bool mIsVisible = true;
        bool mIsInBounds = false;

        void InitializeOptions() {
            mEventSystem.DeregisterListener();

            if (HasOption(DOption::Selectable))
                RegisterOption(DOption::Selectable);
            if (HasOption(DOption::Movable))
                RegisterOption(DOption::Movable);
            if (HasOption(DOption::Resizable))
                RegisterOption(DOption::Resizable);
        }

        void RegisterOption(DOption flag) {
            switch (flag) {
                case DOption::Selectable:
                    mEventSystem.RegisterListener(DEventType::MouseDown, [this](const DEvent& event) {
                        mIsSelected = event.mIsInBounds;
                    });

                    break;

                case DOption::Movable:
                    mEventSystem.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                        if (event.mIsInBounds == true) {
                            mIsMoving = true;
                            mOffsetOption = event.mControls.mMouse.mPosition - event.mPosition;
                        }
                    });
                    mEventSystem.RegisterListener(DEventType::MouseMove, [this](const DEvent& event) {
                        if (mIsMoving == true) {
                            SetPosition(event.mControls.mMouse.mPosition - mOffsetOption);
                        }
                    });
                    mEventSystem.RegisterListener(DEventType::MouseLUp, [this](const DEvent&) {
                        mIsMoving = false;
                    });

                    break;

                case DOption::Resizable:
                    mEventSystem.RegisterListener(DEventType::MouseRDown, [this](const DEvent& event) {
                        sf::Vector2f window_br = event.mPosition + event.mSize;
                        sf::Vector2f size_handle = {10.0f, 10.0f};
                        sf::Vector2f pos_handle = {window_br.x - 10.0f, window_br.y - 10.0f};

                        if (sf::Rect<float>(pos_handle, size_handle).contains(event.mControls.mMouse.mPosition) == true) {
                            mIsResizing = true;
                            mOffsetOption = event.mControls.mMouse.mPosition - window_br;
                        }
                    });
                    mEventSystem.RegisterListener(DEventType::MouseMove, [this](const DEvent& event) {
                        if (mIsResizing == true) {
                            sf::Vector2f pos_new_br = event.mControls.mMouse.mPosition - mOffsetOption;
                            sf::Vector2f size_new = pos_new_br - event.mPosition;
                            sf::Vector2f minimum_size = {10, 10};

                            size_new.x = std::max(size_new.x, minimum_size.x);
                            size_new.y = std::max(size_new.y, minimum_size.y);
                            SetSize(size_new);
                        }
                    });
                    mEventSystem.RegisterListener(DEventType::MouseRUp, [this](const DEvent&) {
                        mIsResizing = false;
                    });

                    break;

                default:
                    break;
            }
        }

    protected:
        using Self = Derived;

        DermaEventSystem mEventSystem;

        Derived& self() {
            return *static_cast<Derived*>(this);
        }

        const Derived& self() const {
            return *static_cast<const Derived*>(this);
        }

    public:
        explicit Derma(size_t id) : mID(id) {}

        Derived& AddOption(DOption flag) {
            if ((mOptionFlag & flag) == DOption::None) {
                mOptionFlag = static_cast<DOption>(static_cast<uint32_t>(mOptionFlag) | static_cast<uint32_t>(flag));

                RegisterOption(flag);
            }

            return self();
        }

        Derived& RemoveOption(DOption flag) {
            if ((mOptionFlag & flag) == flag) {
                mOptionFlag = static_cast<DOption>(static_cast<uint32_t>(mOptionFlag) & ~static_cast<uint32_t>(flag));

                InitializeOptions();
            }

            return self();
        }

        Derived& SetOptions(DOption flags) {
            mOptionFlag = flags;

            InitializeOptions();

            return self();
        }

        bool HasOption(DOption flag) const noexcept {
            return (mOptionFlag & flag) == flag;
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
            return std::dynamic_pointer_cast<T>(DermaBase::shared_from_this());
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
            if (auto parent = mParent.lock())
                return parent->GetPositionGlobal() + mPosition;

            return mPosition;
        }

        size_t GetZLevel() const {
            return mZLevel;
        }

        bool GetVisiblityStatus() const {
            return mIsVisible;
        }

        Derived& SetName(const std::string& name) {
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

        void Update(const Controls& controls) override {
            if (mIsVisible == false)
                return;

            ProcessControls(controls);

            for (auto& child : mChildren) {
                child->Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) override {
            if (mIsVisible == false)
                return;

            sf::Vector2f pos_global = GetPositionGlobal();

            for (const auto& [zlevel, drawing] : mDrawings) {
                ProcessDrawings(window, drawing, pos_global);
            }

            for (auto& child : mChildren) {
                child->Render(window);
            }
        }

        void ProcessControls(const Controls& controls) {
            mIsInBounds = IsInBounds(controls.mMouse.mPosition);

            DEvent event_base;
            event_base.mPosition = GetPositionGlobal();
            event_base.mSize = mSize;
            event_base.mZLevel = mZLevel;
            event_base.mIsInBounds = mIsInBounds;
            event_base.mControls.mMouse.mPosition = controls.mMouse.mPosition;
            event_base.mControls.mMouse.mLPress = controls.mMouse.mLPress;
            event_base.mControls.mMouse.mRPress = controls.mMouse.mRPress;
            event_base.mControls.mMouse.mWPress = controls.mMouse.mWPress;
            event_base.mIsVisible = mIsVisible;

            DEvent event_move = event_base;
            event_move.mType = DEventType::MouseMove;
            mEventSystem.EmitEvent(event_move);

            if ((event_base.mControls.mMouse.mLPress == true) && (mControlsPrevious.mMouse.mLPress == false)) {
                DEvent event_mouse_down = event_base;

                event_mouse_down.mType = DEventType::MouseLDown;
                mEventSystem.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mLPress == false) && (mControlsPrevious.mMouse.mLPress == true)) {
                DEvent event_mouse_up = event_base;

                event_mouse_up.mType = DEventType::MouseLUp;
                mEventSystem.EmitEvent(event_mouse_up);
            }

            if ((event_base.mControls.mMouse.mRPress == true) && (mControlsPrevious.mMouse.mRPress == false)) {
                DEvent event_mouse_down = event_base;

                event_mouse_down.mType = DEventType::MouseRDown;
                mEventSystem.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mRPress == false) && (mControlsPrevious.mMouse.mRPress == true)) {
                DEvent event_mouse_up = event_base;

                event_mouse_up.mType = DEventType::MouseRUp;
                mEventSystem.EmitEvent(event_mouse_up);
            }

            mControlsPrevious.mMouse.mLPress = event_base.mControls.mMouse.mLPress;
            mControlsPrevious.mMouse.mRPress = event_base.mControls.mMouse.mRPress;
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
            }
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
            mDrawings.emplace(zlevel, drawing);

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
            drawing->mID = std::move(id);
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

            return self();
        }

        Derived& DrawText(
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            const std::string& text = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType = DDrawingsType::Text;
            drawing->mID = "";
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& DrawTextDynamic(
            const std::string& id,
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            const std::string& text = "") {
            ClearDrawing(id);

            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType = DDrawingsType::Text;
            drawing->mID = std::move(id);
            drawing->mFont = font;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFontSize = font_size;
            drawing->mText = std::move(text);
            mDrawings.emplace(zlevel, drawing);

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
            mDrawings.emplace(zlevel, drawing);

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
            drawing->mID = std::move(id);
            drawing->mSize = size;
            drawing->mPosition = position;
            drawing->mZLevel = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mTexture = texture;
            drawing->mTextureSmoothing = smoothing_enabled;
            mDrawings.emplace(zlevel, drawing);

            return self();
        }

        Derived& ClearDrawing(const std::string& id) {
            if (id.empty() == true) {
                return self();
            }

            std::vector<std::multimap<size_t, std::shared_ptr<DermaDrawings>>::iterator> to_erase;

            for (auto iter = mDrawings.begin(); iter != mDrawings.end(); ++iter) {
                if (iter->second->mID == id) {
                    to_erase.push_back(iter);
                }
            }

            for (auto& iter : to_erase) {
                mDrawings.erase(iter);
            }

            return self();
        }

        Derived& ClearDrawingsAll() {
            mDrawings.clear();

            return self();
        }
    };
}