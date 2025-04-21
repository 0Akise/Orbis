#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "Orbis/Derma/DermaDrawings.hpp"
#include "Orbis/Derma/DermaEvent.hpp"

namespace Orbis {
    class DermaInterface {
    public:
        virtual ~DermaInterface() = default;

        virtual void SetParent(std::shared_ptr<DermaInterface> parent) = 0;
        virtual void AddChild(std::shared_ptr<DermaInterface> child) = 0;
        virtual void RemoveChild(const std::shared_ptr<DermaInterface>& child) = 0;
        virtual std::shared_ptr<DermaInterface> GetParent() const = 0;
        virtual const std::vector<std::shared_ptr<DermaInterface>>& GetChildren() const = 0;

        virtual size_t GetID() const = 0;
        virtual const std::string& GetName() const = 0;
        virtual sf::Vector2f GetPositionLocal() const = 0;
        virtual sf::Vector2f GetPositionGlobal() const = 0;

        virtual void Update(const Controls& controls) = 0;
        virtual void Render(sf::RenderWindow& window) = 0;
        virtual void Initialize() = 0;
    };
}