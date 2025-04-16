#pragma once

#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Controls.hpp"
#include "Orbis/Data.hpp"
#include "Orbis/DermaEvent.hpp"
#include "Orbis/DermaInterface.hpp"

namespace Orbis {
    class DermaOption {
    public:
        virtual ~DermaOption() = default;

        virtual void Initialize(DermaEventSystem& event, NotifyCallback notify) = 0;
        virtual void Update(const Controls&) {};
        virtual void Render(sf::RenderWindow&) {};
    };

    class Selectable : public DermaOption {
    private:
        [[maybe_unused]] std::weak_ptr<DermaInterface> mParent;
        bool mIsSelected;
        bool mIsInBounds;

    public:
        explicit Selectable(
            std::shared_ptr<DermaInterface> derma, bool is_in_bounds)
            : mParent(derma), mIsSelected(false), mIsInBounds(is_in_bounds) {}

        bool GetSelectedStatus() const {
            return mIsSelected;
        }

        void SetInBoundsStatus(bool is_in_bounds) {
            mIsInBounds = is_in_bounds;
        }

        void Initialize(DermaEventSystem& event_system, NotifyCallback) override {
            event_system.RegisterListener(DEventType::MouseDown, [this](const DEvent&) {
                if (mIsInBounds == true) {
                    mIsSelected = true;
                } else {
                    mIsSelected = false;
                }
            });
        }
    };

    class Movable : public DermaOption {
    private:
        [[maybe_unused]] std::weak_ptr<DermaInterface> mParent;
        sf::Vector2f mPosition;
        sf::Vector2f mOffsetMoving;
        bool mIsMoved;
        bool mIsInBounds;

    public:
        explicit Movable(std::shared_ptr<DermaInterface> derma, sf::Vector2f position, bool is_in_bounds)
            : mParent(derma), mPosition(position), mIsMoved(false), mIsInBounds(is_in_bounds) {}

        bool GetMovedStatus() const {
            return mIsMoved;
        }

        void SetInBoundsStatus(bool is_in_bounds) {
            mIsInBounds = is_in_bounds;
        }

        void Initialize(DermaEventSystem& event_system, NotifyCallback notify) override {
            auto notify_ref = notify;

            event_system.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                if (mIsInBounds == true) {
                    mIsMoved = true;

                    if (auto parent = mParent.lock(); parent != nullptr) {
                        sf::Vector2f pos_global = parent->GetPositionGlobal();
                        mOffsetMoving = event.mMouseState.mPosition - pos_global;
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseMove, [this, notify_ref](const DEvent& event) {
                if (mIsMoved == true) {
                    if (auto parent = mParent.lock(); parent != nullptr) {
                        sf::Vector2f pos_global = parent->GetPositionGlobal();
                        mPosition = event.mMouseState.mPosition - pos_global;

                        notify_ref(DEventType::Moved, &mPosition);
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseLUp, [this](const DEvent&) {
                mIsMoved = false;
            });
        }
    };

    class Resizable : public DermaOption {
    private:
        [[maybe_unused]] std::weak_ptr<DermaInterface> mParent;
        sf::Vector2f mPosition;
        sf::Vector2f mSize;
        sf::Vector2f mOffsetResizing;
        sf::Vector2f mMinimumSize = {10.0f, 10.0f};
        bool mIsResized = false;
        bool mIsDebugMode;

    public:
        explicit Resizable(std::shared_ptr<DermaInterface> derma, sf::Vector2f position, sf::Vector2f size, bool is_debug_mode)
            : mParent(derma), mPosition(position), mSize(size), mIsDebugMode(is_debug_mode) {}

        bool GetResizedStatus() const {
            return mIsResized;
        }

        sf::Vector2f GetMinimumSize() const {
            return mMinimumSize;
        }

        void SetMinimumSize(sf::Vector2f size) {
            mMinimumSize = size;
        }

        void Initialize(DermaEventSystem& event_system, NotifyCallback notify) override {
            auto notify_ref = notify;
            auto is_cursor_in_resize_handle = [this](const sf::Vector2f& pos_mouse) -> bool {
                if (auto parent = mParent.lock(); parent != nullptr) {
                    sf::Vector2f pos_global = parent->GetPositionGlobal();
                    sf::Vector2f br_derma = pos_global + mSize;
                    sf::Vector2f size_handle = {10.0f, 10.0f};
                    sf::Vector2f pos_handle = {br_derma.x - size_handle.x, br_derma.y - size_handle.y};

                    return sf::Rect<float>(pos_handle, size_handle).contains(pos_mouse);
                }

                return false;
            };

            event_system.RegisterListener(DEventType::MouseRDown, [this, is_cursor_in_resize_handle](const DEvent& event) {
                if (is_cursor_in_resize_handle(event.mMouseState.mPosition) == true) {
                    mIsResized = true;

                    if (auto parent = mParent.lock(); parent != nullptr) {
                        sf::Vector2f pos_global = parent->GetPositionGlobal();
                        sf::Vector2f br_derma = pos_global + mSize;
                        mOffsetResizing = event.mMouseState.mPosition - br_derma;
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseMove, [this, notify_ref](const DEvent& event) {
                if (mIsResized == true) {
                    if (auto parent = mParent.lock(); parent != nullptr) {
                        sf::Vector2f pos_global = parent->GetPositionGlobal();
                        sf::Vector2f size_new = (event.mMouseState.mPosition - mOffsetResizing) - pos_global;

                        size_new.x = std::max(size_new.x, mMinimumSize.x);
                        size_new.y = std::max(size_new.y, mMinimumSize.y);
                        mSize = size_new;

                        notify_ref(DEventType::Resized, &mSize);
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseRUp, [this](const DEvent&) {
                mIsResized = false;
            });
        }

        void Render(sf::RenderWindow& window) override {
            if (mIsDebugMode == true) {
                if (auto parent = mParent.lock(); parent != nullptr) {
                    sf::Vector2f pos_global = parent->GetPositionGlobal();
                    sf::RectangleShape handle({10.0f, 10.0f});

                    handle.setPosition({(pos_global.x + mSize.x) - 10.0f, (pos_global.y + mSize.y) - 10.0f});
                    handle.setFillColor(sf::Color(200, 200, 200));
                    window.draw(handle);
                }
            }
        }
    };
}