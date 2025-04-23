#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "Orbis/Base/Controls.hpp"

namespace Orbis {
    class DermaBase : public std::enable_shared_from_this<DermaBase> {
    protected:
        std::weak_ptr<DermaBase> mParent;
        std::vector<std::shared_ptr<DermaBase>> mChildren;

    public:
        virtual ~DermaBase() = default;

        void SetParent(const std::shared_ptr<DermaBase>& parent) {
            mParent = parent;
        }

        void AddChild(const std::shared_ptr<DermaBase>& child) {
            if (auto parent = child->mParent.lock()) {
                parent->RemoveChild(child);
            }

            child->mParent = shared_from_this();

            mChildren.push_back(child);
        }

        void RemoveChild(const std::shared_ptr<DermaBase>& child) {
            auto iter = std::find(mChildren.begin(), mChildren.end(), child);

            if (iter != mChildren.end()) {
                (*iter)->mParent.reset();

                mChildren.erase(iter);
            }
        }

        std::shared_ptr<DermaBase> GetParent() const {
            return mParent.lock();
        }

        const std::vector<std::shared_ptr<DermaBase>>& GetChildren() const {
            return mChildren;
        }

        virtual size_t GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual sf::Vector2f GetPositionLocal() const = 0;
        virtual sf::Vector2f GetPositionGlobal() const = 0;
        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow& window) = 0;
    };
}