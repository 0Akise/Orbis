#pragma once

#include <functional>
#include <memory>

#include <SFML/Graphics.hpp>

namespace Orbis {
    template <typename T>
    class WidgetHandle {
    private:
        std::shared_ptr<T> mWidget;

    public:
        WidgetHandle(std::shared_ptr<T> widget) : mWidget(widget) {}

        T* operator->() {
            return mWidget.get();
        }

        T& operator*() {
            return *mWidget;
        }

        T& GetWidget() {
            return *mWidget;
        }

        std::shared_ptr<WidgetInterface> GetWidgetShared() const {
            return std::static_pointer_cast<WidgetInterface>(mWidget);
        }

        WidgetHandle& SetSize(sf::Vector2f size) {
            mWidget->SetSize(size);

            return *this;
        }

        WidgetHandle& SetZLevel(size_t zlevel) {
            mWidget->SetZLevel(zlevel);

            return *this;
        }

        WidgetHandle& SetVisible(bool is_visible) {
            mWidget->SetVisible(is_visible);

            return *this;
        }

        auto SetText(const std::string& text) -> decltype(mWidget->SetText(text), *this) {
            mWidget->SetText(text);

            return *this;
        }

        auto SetCallback(std::function<void()> callback) -> decltype(mWidget->SetCallback(callback), *this) {
            mWidget->SetCallback(callback);

            return *this;
        }

        WidgetHandle& DrawRect(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            bool is_outlined = false,
            float outline_thickness = 0.0f,
            sf::Color outline_color = sf::Color::Black,
            bool is_rounded = false,
            float rounding_radius = 0.0f) {
            mWidget->DrawRect(size, position, zlevel, fill_color, is_outlined, outline_thickness, outline_color, is_rounded, rounding_radius);

            return *this;
        }

        WidgetHandle& DrawText(
            sf::Font font,
            size_t font_size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            const std::string& text = "") {
            mWidget->DrawText(font, font_size, position, zlevel, fill_color, text);

            return *this;
        }

        WidgetHandle& DrawTexture(
            sf::Vector2f size,
            sf::Vector2f position,
            size_t zlevel,
            sf::Color fill_color,
            sf::Texture texture,
            bool smoothing_enabled = true) {
            mWidget->Drawtexture(size, position, zlevel, fill_color, texture, smoothing_enabled);

            return *this;
        }
    };
}