#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "Orbis/Base/Controls.hpp"
#include "Orbis/UIEventDispatcher.hpp"

namespace Orbis {
    class DermaBase : public std::enable_shared_from_this<DermaBase> {
    protected:
        std::weak_ptr<DermaBase> mParent;
        std::vector<std::shared_ptr<DermaBase>> mChildren;

        bool mIsUpdating = false;
        std::vector<std::shared_ptr<DermaBase>> mPendingChildrenToAdd;
        std::vector<std::shared_ptr<DermaBase>> mPendingChildrenToRemove;

        size_t mZLevelBase = 0;
        size_t mZLevelRuntime = 0;

    public:
        virtual ~DermaBase() = default;

        void AddChild(const std::shared_ptr<DermaBase>& child) {
            if (mIsUpdating) {
                mPendingChildrenToAdd.push_back(child);

                return;
            }

            if (auto parent = child->mParent.lock()) {
                parent->RemoveChild(child);
            }

            child->SetParent(shared_from_this());

            mChildren.push_back(child);
        }

        void RemoveChild(const std::shared_ptr<DermaBase>& child) {
            if (mIsUpdating) {
                mPendingChildrenToRemove.push_back(child);

                return;
            }

            auto iter = std::find(mChildren.begin(), mChildren.end(), child);

            if (iter != mChildren.end()) {
                (*iter)->ResetParent();

                mChildren.erase(iter);
            }
        }

        void ProcessPendingChildren() {
            for (auto& child : mPendingChildrenToRemove) {
                RemoveChild(child);
            }

            mPendingChildrenToRemove.clear();

            for (auto& child : mPendingChildrenToAdd) {
                AddChild(child);
            }

            mPendingChildrenToAdd.clear();
        }

        bool IsChildOf(const std::shared_ptr<DermaBase>& parent) const {
            if (parent == nullptr)
                return false;

            std::shared_ptr<DermaBase> parent_current = mParent.lock();
            while (parent_current) {
                if (parent_current == parent) {
                    return true;
                }

                parent_current = parent_current->mParent.lock();
            }

            return false;
        }

        void SetParent(const std::shared_ptr<DermaBase>& parent) {
            mParent = parent;
        }

        void ResetParent() {
            mParent.reset();
        }

        std::shared_ptr<DermaBase> GetParent() const {
            return mParent.lock();
        }

        const std::vector<std::shared_ptr<DermaBase>>& GetChildren() const {
            return mChildren;
        }

        size_t GetZLevelBase() const {
            return mZLevelBase;
        }

        size_t GetZLevelRuntime() const {
            return mZLevelRuntime;
        }

        void SetZLevelBase(int z_level) {
            mZLevelBase = z_level;

            UIEventDispatcher::Get().NotifyZLevelChanged();
        }

        void SetZLevelRuntime(int z_level) {
            mZLevelRuntime = z_level;
        }

        virtual size_t GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual sf::Vector2f GetPositionLocal() const = 0;
        virtual sf::Vector2f GetPositionGlobal() const = 0;
        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow& window) = 0;
    };
}