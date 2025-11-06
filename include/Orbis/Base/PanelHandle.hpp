#pragma once

#include <memory>

#include "Orbis/Base/Panel.hpp"

namespace Orbis {
    class PanelHandle {
    private:
        std::shared_ptr<Panel> mPanel;

    public:
        PanelHandle(std::shared_ptr<Panel> panel)
            : mPanel(panel) {}

        Panel* operator->() {
            return mPanel.get();
        }

        Panel& operator*() {
            return *mPanel;
        }

        Panel& GetDerma() {
            return *mPanel;
        }

        std::shared_ptr<Panel> GetDermaShared() const {
            return mPanel;
        }

        template <typename WidgetHandleT>
        PanelHandle& AddWidget(const WidgetHandleT& widget_handle, const sf::Vector2f& position, size_t z_level = 0) {
            mPanel->AddWidget(widget_handle, position, z_level);

            return *this;
        }

        PanelHandle& SetName(const std::string& name) {
            mPanel->SetName(name);

            return *this;
        }

        PanelHandle& SetSize(const sf::Vector2f& size) {
            mPanel->SetSize(size);

            return *this;
        }

        PanelHandle& SetPosition(const sf::Vector2f& position) {
            mPanel->SetPosition(position);

            return *this;
        }

        PanelHandle& SetZLevel(size_t z_level) {
            mPanel->SetZLevel(z_level);

            return *this;
        }

        PanelHandle& SetVisible(bool is_visible) {
            mPanel->SetVisible(is_visible);

            return *this;
        }

        PanelHandle& SetOptions(PanelOption flags) {
            mPanel->SetOptions(flags);

            return *this;
        }
    };
}