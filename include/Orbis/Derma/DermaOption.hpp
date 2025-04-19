#pragma once

#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Derma/DermaEvent.hpp"
#include "Orbis/Derma/DermaInterface.hpp"

namespace Orbis {
    class DermaOption {
    public:
        virtual ~DermaOption() = default;

        virtual void Initialize(DermaEventSystem& event, NotifyCallback notify) = 0;
    };

    class Selectable : public DermaOption {
    private:
        [[maybe_unused]] std::weak_ptr<DermaInterface> mParent;
        bool mIsSelected;

    public:
        explicit Selectable(
            std::shared_ptr<DermaInterface> derma)
            : mParent(derma), mIsSelected(false) {}

        bool GetSelectedStatus() const {
            return mIsSelected;
        }

        void Initialize(DermaEventSystem& event_system, NotifyCallback) override {
            event_system.RegisterListener(DEventType::MouseDown, [this](const DEvent& event) {
                if (event.mIsInBounds == true) {
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

    public:
        explicit Movable(std::shared_ptr<DermaInterface> derma, sf::Vector2f position)
            : mParent(derma), mPosition(position), mIsMoved(false) {}

        bool GetMovedStatus() const {
            return mIsMoved;
        }

        void Initialize(DermaEventSystem& event_system, NotifyCallback notify) override {
            auto notify_ref = notify;

            event_system.RegisterListener(DEventType::MouseLDown, [this](const DEvent& event) {
                if (event.mIsInBounds == true) {
                    mIsMoved = true;

                    if (auto parent = mParent.lock(); parent != nullptr) {
                        mOffsetMoving = event.mControls.mMouse.mPosition - event.mPosition;
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseMove, [this, notify_ref](const DEvent& event) {
                if (mIsMoved == true) {
                    if (auto parent = mParent.lock(); parent != nullptr) {
                        mPosition = event.mControls.mMouse.mPosition - mOffsetMoving;

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
        sf::Vector2f mSize;
        sf::Vector2f mOffsetResizing;
        sf::Vector2f mMinimumSize = {10.0f, 10.0f};
        bool mIsResized = false;

    public:
        explicit Resizable(std::shared_ptr<DermaInterface> derma, sf::Vector2f size)
            : mParent(derma), mSize(size) {}

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

            event_system.RegisterListener(DEventType::MouseRDown, [this](const DEvent& event) {
                sf::Vector2f window_br = event.mPosition + event.mSize;
                sf::Vector2f size_handle = {10.0f, 10.0f};
                sf::Vector2f pos_handle = {window_br.x - 10.0f, window_br.y - 10.0f};

                if (sf::Rect<float>(pos_handle, size_handle).contains(event.mControls.mMouse.mPosition) == true) {
                    mIsResized = true;

                    if (auto parent = mParent.lock(); parent != nullptr) {
                        mOffsetResizing = event.mControls.mMouse.mPosition - window_br;
                    }
                }
            });

            event_system.RegisterListener(DEventType::MouseMove, [this, notify_ref](const DEvent& event) {
                if (mIsResized == true) {
                    if (auto parent = mParent.lock(); parent != nullptr) {
                        sf::Vector2f pos_new_br = event.mControls.mMouse.mPosition - mOffsetResizing;
                        sf::Vector2f size_new = pos_new_br - event.mPosition;

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
    };
}