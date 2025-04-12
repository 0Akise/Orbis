#pragma once

#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/DermaEvent.hpp"

namespace Orbis {
    class DermaComponent {
    public:
        virtual ~DermaComponent() = default;

        using NotifyCallback = std::function<void(DermaEventType, const void*)>;

        virtual void Initialize(EventSystem& event, NotifyCallback notify) = 0;
        virtual void Update(const Controls&) {};
        virtual void Render(sf::RenderWindow&) {};
    };

    class Selectable : public DermaComponent {
    private:
        bool mIsSelected;
        bool* mIsInBounds;

    public:
        explicit Selectable(bool& is_in_bounds)
            : mIsInBounds(&is_in_bounds) {}

        bool GetSelectedStatus() const {
            return mIsSelected;
        }

        void Initialize(EventSystem& event_system, NotifyCallback) override {
            event_system.RegisterListener(DermaEventType::MouseDown, [this](const DermaEvent&) {
                if (*mIsInBounds == true) {
                    mIsSelected = true;
                } else {
                    mIsSelected = false;
                }
            });
        }
    };

    class Movable : public DermaComponent {
    private:
        bool mIsMoving;
        bool* mIsInBounds;
        sf::Vector2f* mPosition;
        sf::Vector2f mOffsetMoving;

    public:
        explicit Movable(sf::Vector2f& position, bool& is_in_bounds)
            : mPosition(&position), mIsInBounds(&is_in_bounds) {}

        bool GetMovingStatus() const {
            return mIsMoving;
        }

        void Initialize(EventSystem& event_system, NotifyCallback notify) override {
            auto notify_ref = notify;

            event_system.RegisterListener(DermaEventType::MouseDown, [this](const DermaEvent& event) {
                if (*mIsInBounds == true) {
                    mIsMoving = true;
                    mOffsetMoving = event.mPositionMouse - *mPosition;
                }
            });

            event_system.RegisterListener(DermaEventType::MouseMove, [this, notify_ref](const DermaEvent& event) {
                if (mIsMoving) {
                    *mPosition = event.mPositionMouse - mOffsetMoving;

                    notify_ref(DermaEventType::Moved, mPosition);
                }
            });

            event_system.RegisterListener(DermaEventType::MouseUp, [this](const DermaEvent&) {
                mIsMoving = false;
            });
        }
    };

    class Resizable : public DermaComponent {
    private:
        bool mIsResizing = false;
        bool* mIsDebugMode;
        sf::Vector2f* mPosition;
        sf::Vector2f* mSize;
        sf::Vector2f mOffsetResizing;
        sf::Vector2f mMinimumSize = {10.0f, 10.0f};

    public:
        explicit Resizable(sf::Vector2f& position, sf::Vector2f& size, bool& is_debug_mode)
            : mPosition(&position), mSize(&size), mIsDebugMode(&is_debug_mode) {}

        bool GetResizingStatus() const {
            return mIsResizing;
        }

        sf::Vector2f GetMinimumSize() const {
            return mMinimumSize;
        }

        void SetMinimumSize(sf::Vector2f size) {
            mMinimumSize = size;
        }

        void Initialize(EventSystem& event_system) override {
            auto is_cursor_in_resize_handle = [this](const sf::Vector2f& pos_mouse) -> bool {
                sf::Vector2f br_derma = *mPosition + *mSize;
                sf::Vector2f size_handle = {10.0f, 10.0f};
                sf::Vector2f pos_handle = {br_derma.x - size_handle.x, br_derma.y - size_handle.y};

                return sf::Rect<float>(pos_handle, size_handle).contains(pos_mouse);
            };

            event_system.RegisterListener(DermaEventType::MouseDown, [this, is_cursor_in_resize_handle](const DermaEvent& event) {
                if ((event.mIsRMousePressed == true) && (is_cursor_in_resize_handle(event.mPositionMouse))) {
                    mIsResizing = true;
                    sf::Vector2f br_derma = *mPosition + *mSize;

                    mOffsetResizing = event.mPositionMouse - br_derma;
                }
            });

            event_system.RegisterListener(DermaEventType::MouseMove, [this](const DermaEvent& event) {
                if (mIsResizing == true) {
                    sf::Vector2f size_new = (event.mPositionMouse - mOffsetResizing) - *mPosition;

                    size_new.x = std::max(size_new.x, mMinimumSize.x);
                    size_new.y = std::max(size_new.y, mMinimumSize.y);
                    *mSize = size_new;

                    DermaEvent event_resized;

                    event_resized.mType = DermaEventType::Resized;
                    event_resized.mSize = *mSize;
                    event_system.EmitEvent(event_resized);
                }
            });

            event_system.RegisterListener(DermaEventType::MouseUp, [this](const DermaEvent&) {
                mIsResizing = false;
            });
        }

        void Render(sf::RenderWindow& window) override {
            if (*mIsDebugMode == true) {
                sf::RectangleShape handle({10.0f, 10.0f});

                handle.setPosition({mPosition->x + mSize->x - 10.0f, mPosition->y + mSize->y - 10.0f});
                handle.setFillColor(sf::Color(200, 200, 200));
                window.draw(handle);
            }
        }
    };
}