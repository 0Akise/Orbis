#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Orbis/Base/DermaEvent.hpp"
#include "Orbis/Base/Drawing.hpp"
#include "Orbis/Base/WidgetInterface.hpp"

namespace Orbis {
    enum class DermaOption : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = Selectable | Movable | Resizable
    };

    inline DermaOption operator|(DermaOption a, DermaOption b) {
        return static_cast<DermaOption>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DermaOption operator&(DermaOption a, DermaOption b) {
        return static_cast<DermaOption>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    class Derma {
    private:
        size_t mID;
        std::string mName = "Unnamed";
        sf::Vector2f mSize = {0, 0};
        sf::Vector2f mPosition = {0, 0};
        size_t mZLevel = 0;

        DermaEventHandler mEventHandler;

        DermaOption mOptionFlag = DermaOption::None;
        sf::Vector2f mOffset;
        bool mIsSelected = false;
        bool mIsMoving = false;
        bool mIsResizing = false;

        std::multimap<size_t, std::shared_ptr<WidgetInterface>> mWidgets;
        std::multimap<size_t, std::shared_ptr<Drawing>> mDrawings;

        bool mIsInBounds = false;
        Controls mControlsPrevious;

        bool mIsVisible = true;

    public:
        Derma& AddWidget(const auto& widget_handle, const sf::Vector2f& position, size_t z_level = 0) {
            auto widget_ptr = widget_handle.GetWidgetShared();

            widget_ptr->SetPositionOffset(position);

            mWidgets.emplace(z_level, widget_ptr);

            return *this;
        }

        Derma& MoveWidget(const auto& widget_handle, const sf::Vector2f& position) {
            auto widget_ptr = widget_handle.GetWidgetShared();

            widget_ptr->SetPositionOffset(position);
        }

        void InitializeOptions() {
            mEventHandler.DeregisterListener();

            if (HasOption(DermaOption::Selectable))
                RegisterOption(DermaOption::Selectable);
            if (HasOption(DermaOption::Movable))
                RegisterOption(DermaOption::Movable);
            if (HasOption(DermaOption::Resizable))
                RegisterOption(DermaOption::Resizable);
        }

        void RegisterOption(DermaOption flag) {
            switch (flag) {
                case DermaOption::Selectable:
                    mEventHandler.RegisterListener(EventType::MouseDown, [this](const DermaEvent& event) {
                        mIsSelected = event.mIsInBounds;
                    });

                    mEventHandler.RegisterListener(EventType::MouseDown, [this](const DermaEvent& event) {
                        if (event.mIsInBounds) {
                            mIsSelected = true;
                        }
                    });

                    break;

                case DermaOption::Movable:
                    mEventHandler.RegisterListener(EventType::MouseLDown, [this](const DermaEvent& event) {
                        if (event.mIsInBounds == true) {
                            mIsMoving = true;
                            mOffset = event.mControls.mMouse.mPosition - event.mPosition;
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseMove, [this](const DermaEvent& event) {
                        if (mIsMoving == true) {
                            SetPosition(event.mControls.mMouse.mPosition - mOffset);
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseLUp, [this](const DermaEvent&) {
                        mIsMoving = false;
                    });

                    break;

                case DermaOption::Resizable:
                    mEventHandler.RegisterListener(EventType::MouseRDown, [this](const DermaEvent& event) {
                        sf::Vector2f window_br = event.mPosition + event.mSize;
                        sf::Vector2f size_handle = {10.0f, 10.0f};
                        sf::Vector2f pos_handle = {window_br.x - 10.0f, window_br.y - 10.0f};

                        if (sf::Rect<float>(pos_handle, size_handle).contains(event.mControls.mMouse.mPosition) == true) {
                            mIsResizing = true;
                            mOffset = event.mControls.mMouse.mPosition - window_br;
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseMove, [this](const DermaEvent& event) {
                        if (mIsResizing == true) {
                            sf::Vector2f pos_new_br = event.mControls.mMouse.mPosition - mOffset;
                            sf::Vector2f size_new = pos_new_br - event.mPosition;
                            sf::Vector2f minimum_size = {10, 10};

                            size_new.x = std::max(size_new.x, minimum_size.x);
                            size_new.y = std::max(size_new.y, minimum_size.y);
                            SetSize(size_new);
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseRUp, [this](const DermaEvent&) {
                        mIsResizing = false;
                    });

                    break;

                default:
                    break;
            }
        }

        Derma& AddOption(DermaOption flag) {
            if ((mOptionFlag & flag) == DermaOption::None) {
                mOptionFlag = static_cast<DermaOption>(static_cast<uint32_t>(mOptionFlag) | static_cast<uint32_t>(flag));

                RegisterOption(flag);
            }

            return *this;
        }

        Derma& RemoveOption(DermaOption flag) {
            if ((mOptionFlag & flag) == flag) {
                mOptionFlag = static_cast<DermaOption>(static_cast<uint32_t>(mOptionFlag) & ~static_cast<uint32_t>(flag));

                InitializeOptions();
            }

            return *this;
        }

        Derma& SetOptions(DermaOption flags) {
            mOptionFlag = flags;

            InitializeOptions();

            return *this;
        }

        bool HasOption(DermaOption flag) const noexcept {
            return (mOptionFlag & flag) == flag;
        }

        bool HasOptionFlag(DermaOption flags, DermaOption flag) const {
            return (flags & flag) == flag;
        }

        size_t GetID() const {
            return mID;
        }

        const std::string& GetName() const {
            return mName;
        }

        sf::Vector2f GetPositionLocal() const {
            return mPosition;
        }

        sf::Vector2f GetPositionGlobal() const {
            return mPosition;
        }

        bool IsInBounds(sf::Vector2f cursor_position) const {
            sf::Vector2f pos_global = GetPositionGlobal();

            return ((pos_global.x <= cursor_position.x) && (cursor_position.x <= pos_global.x + mSize.x)) &&
                   ((pos_global.y <= cursor_position.y) && (cursor_position.y <= pos_global.y + mSize.y));
        }

        Derma& SetID(size_t id) {
            mID = id;

            return *this;
        }

        Derma& SetName(const std::string& name) {
            mName = name;

            return *this;
        }

        Derma& SetSize(const sf::Vector2f& size) {
            mSize = size;

            return *this;
        }

        Derma& SetPosition(const sf::Vector2f& position) {
            mPosition = position;

            return *this;
        }

        Derma& SetZLevel(size_t z_level) {
            mZLevel = z_level;

            return *this;
        }

        Derma& SetVisible(bool is_visible) {
            mIsVisible = is_visible;

            return *this;
        }

        void Update(Controls controls) {
            if (mIsVisible == false)
                return;

            ProcessControls(controls);
        }

        void Render(sf::RenderWindow& window) {
            if (mIsVisible == false)
                return;

            for (auto& [id, widget] : mWidgets) {
                widget->Render(window);
            }
        }

        void ProcessControls(const Controls& controls) {
            mIsInBounds = IsInBounds(controls.mMouse.mPosition);

            DermaEvent event_base;
            event_base.mPosition = GetPositionGlobal();
            event_base.mSize = mSize;
            event_base.mZLevel = mZLevel;
            event_base.mIsInBounds = mIsInBounds;
            event_base.mControls.mMouse.mPosition = controls.mMouse.mPosition;
            event_base.mControls.mMouse.mLPress = controls.mMouse.mLPress;
            event_base.mControls.mMouse.mRPress = controls.mMouse.mRPress;
            event_base.mControls.mMouse.mWPress = controls.mMouse.mWPress;
            event_base.mIsVisible = mIsVisible;

            DermaEvent event_move = event_base;
            event_move.mType = EventType::MouseMove;
            mEventHandler.EmitEvent(event_move);

            if ((event_base.mControls.mMouse.mLPress == true) && (mControlsPrevious.mMouse.mLPress == false)) {
                DermaEvent event_mouse_down = event_base;

                event_mouse_down.mType = EventType::MouseLDown;

                mEventHandler.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mLPress == false) && (mControlsPrevious.mMouse.mLPress == true)) {
                DermaEvent event_mouse_up = event_base;

                event_mouse_up.mType = EventType::MouseLUp;
                mEventHandler.EmitEvent(event_mouse_up);
            }

            if ((event_base.mControls.mMouse.mRPress == true) && (mControlsPrevious.mMouse.mRPress == false)) {
                DermaEvent event_mouse_down = event_base;

                event_mouse_down.mType = EventType::MouseRDown;
                mEventHandler.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mRPress == false) && (mControlsPrevious.mMouse.mRPress == true)) {
                DermaEvent event_mouse_up = event_base;

                event_mouse_up.mType = EventType::MouseRUp;
                mEventHandler.EmitEvent(event_mouse_up);
            }

            mControlsPrevious.mMouse.mLPress = event_base.mControls.mMouse.mLPress;
            mControlsPrevious.mMouse.mRPress = event_base.mControls.mMouse.mRPress;
        }
    };
}