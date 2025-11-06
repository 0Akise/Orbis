#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Orbis/Base/Drawing.hpp"
#include "Orbis/Base/PanelEvent.hpp"
#include "Orbis/Base/WidgetInterface.hpp"

namespace Orbis {
    enum class PanelOption : uint32_t {
        None = 0,
        Selectable = 1 << 0,
        Movable = 1 << 1,
        Resizable = 1 << 2,

        Default = Selectable | Movable | Resizable
    };

    inline PanelOption operator|(PanelOption a, PanelOption b) {
        return static_cast<PanelOption>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline PanelOption operator&(PanelOption a, PanelOption b) {
        return static_cast<PanelOption>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    class Panel {
    private:
        size_t mID;
        std::string mName = "Unnamed";
        sf::Vector2f mSize = {0, 0};
        sf::Vector2f mPosition = {0, 0};
        size_t mZLevel = 0;

        PanelEventHandler mEventHandler;

        PanelOption mOptionFlag = PanelOption::None;
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
        Panel& AddWidget(const auto& widget_handle, const sf::Vector2f& position, size_t z_level = 0) {
            auto widget_ptr = widget_handle.GetWidgetShared();

            widget_ptr->SetPositionOffset(position);

            mWidgets.emplace(z_level, widget_ptr);

            return *this;
        }

        Panel& MoveWidget(const auto& widget_handle, const sf::Vector2f& position) {
            auto widget_ptr = widget_handle.GetWidgetShared();

            widget_ptr->SetPositionOffset(position);
        }

        void InitializeOptions() {
            mEventHandler.DeregisterListener();

            if (HasOption(PanelOption::Selectable))
                RegisterOption(PanelOption::Selectable);
            if (HasOption(PanelOption::Movable))
                RegisterOption(PanelOption::Movable);
            if (HasOption(PanelOption::Resizable))
                RegisterOption(PanelOption::Resizable);
        }

        void RegisterOption(PanelOption flag) {
            switch (flag) {
                case PanelOption::Selectable:
                    mEventHandler.RegisterListener(EventType::MouseDown, [this](const PanelEvent& event) {
                        mIsSelected = event.mIsInBounds;
                    });

                    mEventHandler.RegisterListener(EventType::MouseDown, [this](const PanelEvent& event) {
                        if (event.mIsInBounds) {
                            mIsSelected = true;
                        }
                    });

                    break;

                case PanelOption::Movable:
                    mEventHandler.RegisterListener(EventType::MouseLDown, [this](const PanelEvent& event) {
                        if (event.mIsInBounds == true) {
                            mIsMoving = true;
                            mOffset = event.mControls.mMouse.mPosition - event.mPosition;
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseMove, [this](const PanelEvent& event) {
                        if (mIsMoving == true) {
                            SetPosition(event.mControls.mMouse.mPosition - mOffset);
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseLUp, [this](const PanelEvent&) {
                        mIsMoving = false;
                    });

                    break;

                case PanelOption::Resizable:
                    mEventHandler.RegisterListener(EventType::MouseRDown, [this](const PanelEvent& event) {
                        sf::Vector2f window_br = event.mPosition + event.mSize;
                        sf::Vector2f size_handle = {10.0f, 10.0f};
                        sf::Vector2f pos_handle = {window_br.x - 10.0f, window_br.y - 10.0f};

                        if (sf::Rect<float>(pos_handle, size_handle).contains(event.mControls.mMouse.mPosition) == true) {
                            mIsResizing = true;
                            mOffset = event.mControls.mMouse.mPosition - window_br;
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseMove, [this](const PanelEvent& event) {
                        if (mIsResizing == true) {
                            sf::Vector2f pos_new_br = event.mControls.mMouse.mPosition - mOffset;
                            sf::Vector2f size_new = pos_new_br - event.mPosition;
                            sf::Vector2f minimum_size = {10, 10};

                            size_new.x = std::max(size_new.x, minimum_size.x);
                            size_new.y = std::max(size_new.y, minimum_size.y);
                            SetSize(size_new);
                        }
                    });
                    mEventHandler.RegisterListener(EventType::MouseRUp, [this](const PanelEvent&) {
                        mIsResizing = false;
                    });

                    break;

                default:
                    break;
            }
        }

        Panel& AddOption(PanelOption flag) {
            if ((mOptionFlag & flag) == PanelOption::None) {
                mOptionFlag = static_cast<PanelOption>(static_cast<uint32_t>(mOptionFlag) | static_cast<uint32_t>(flag));

                RegisterOption(flag);
            }

            return *this;
        }

        Panel& RemoveOption(PanelOption flag) {
            if ((mOptionFlag & flag) == flag) {
                mOptionFlag = static_cast<PanelOption>(static_cast<uint32_t>(mOptionFlag) & ~static_cast<uint32_t>(flag));

                InitializeOptions();
            }

            return *this;
        }

        Panel& SetOptions(PanelOption flags) {
            mOptionFlag = flags;

            InitializeOptions();

            return *this;
        }

        bool HasOption(PanelOption flag) const noexcept {
            return (mOptionFlag & flag) == flag;
        }

        bool HasOptionFlag(PanelOption flags, PanelOption flag) const {
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

        Panel& SetID(size_t id) {
            mID = id;

            return *this;
        }

        Panel& SetName(const std::string& name) {
            mName = name;

            return *this;
        }

        Panel& SetSize(const sf::Vector2f& size) {
            mSize = size;

            return *this;
        }

        Panel& SetPosition(const sf::Vector2f& position) {
            mPosition = position;

            return *this;
        }

        Panel& SetZLevel(size_t z_level) {
            mZLevel = z_level;

            return *this;
        }

        Panel& SetVisible(bool is_visible) {
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

            PanelEvent event_base;
            event_base.mPosition = GetPositionGlobal();
            event_base.mSize = mSize;
            event_base.mZLevel = mZLevel;
            event_base.mIsInBounds = mIsInBounds;
            event_base.mControls.mMouse.mPosition = controls.mMouse.mPosition;
            event_base.mControls.mMouse.mLPress = controls.mMouse.mLPress;
            event_base.mControls.mMouse.mRPress = controls.mMouse.mRPress;
            event_base.mControls.mMouse.mWPress = controls.mMouse.mWPress;
            event_base.mIsVisible = mIsVisible;

            PanelEvent event_move = event_base;
            event_move.mType = EventType::MouseMove;
            mEventHandler.EmitEvent(event_move);

            if ((event_base.mControls.mMouse.mLPress == true) && (mControlsPrevious.mMouse.mLPress == false)) {
                PanelEvent event_mouse_down = event_base;

                event_mouse_down.mType = EventType::MouseLDown;

                mEventHandler.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mLPress == false) && (mControlsPrevious.mMouse.mLPress == true)) {
                PanelEvent event_mouse_up = event_base;

                event_mouse_up.mType = EventType::MouseLUp;
                mEventHandler.EmitEvent(event_mouse_up);
            }

            if ((event_base.mControls.mMouse.mRPress == true) && (mControlsPrevious.mMouse.mRPress == false)) {
                PanelEvent event_mouse_down = event_base;

                event_mouse_down.mType = EventType::MouseRDown;
                mEventHandler.EmitEvent(event_mouse_down);
            } else if ((event_base.mControls.mMouse.mRPress == false) && (mControlsPrevious.mMouse.mRPress == true)) {
                PanelEvent event_mouse_up = event_base;

                event_mouse_up.mType = EventType::MouseRUp;
                mEventHandler.EmitEvent(event_mouse_up);
            }

            mControlsPrevious.mMouse.mLPress = event_base.mControls.mMouse.mLPress;
            mControlsPrevious.mMouse.mRPress = event_base.mControls.mMouse.mRPress;
        }
    };
}