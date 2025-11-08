#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Controls.hpp"
#include "Orbis/System/Drawing.hpp"
#include "Orbis/System/ResourceVault.hpp"

namespace Orbis {
    enum class WidgetType {
        Canvas,
        Button,
    };

    enum class ButtonState {
        Normal,
        Hover,
        Pressed,
    };

    class UIContext;
    class UI;

    class Widget : public std::enable_shared_from_this<Widget> {
    protected:
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        bool         mIsVisible = true;

        std::map<std::string, std::shared_ptr<DrawingsRect>>    mDrawingRects;
        std::map<std::string, std::shared_ptr<DrawingsText>>    mDrawingTexts;
        std::map<std::string, std::shared_ptr<DrawingsTexture>> mDrawingTextures;

    public:
        virtual ~Widget() = default;

        DrawingsRect& GetRect(const std::string& id) {
            auto iter = mDrawingRects.find(id);

            if (iter == mDrawingRects.end()) {
                throw std::runtime_error("DrawingsRect with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsText& GetText(const std::string& id) {
            auto iter = mDrawingTexts.find(id);

            if (iter == mDrawingTexts.end()) {
                throw std::runtime_error("DrawingsText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsTexture& GetTexture(const std::string& id) {
            auto iter = mDrawingTextures.find(id);

            if (iter == mDrawingTextures.end()) {
                throw std::runtime_error("DrawingsTexture with id '" + id + "' not found");
            }

            return *iter->second;
        }

        sf::Vector2f GetPosition() const {
            return mPosition;
        }

        sf::Vector2f GetSize() const {
            return mSize;
        }

        bool IsVisible() const {
            return mIsVisible;
        }

        Widget& SetSize(sf::Vector2f size) {
            mSize = size;

            return *this;
        }

        Widget& SetPosition(sf::Vector2f position) {
            mPosition = position;

            return *this;
        }

        Widget& SetVisible(bool visible) {
            mIsVisible = visible;

            return *this;
        }

        Widget& DrawRect(
            const std::string& id,
            sf::Vector2f       size,
            sf::Vector2f       position,
            size_t             zlevel,
            sf::Color          fill_color,
            bool               is_outlined       = false,
            float              outline_thickness = 0.0f,
            sf::Color          outline_color     = sf::Color::Black,
            bool               is_rounded        = false,
            float              rounding_radius   = 0.0f) {
            auto drawing = std::make_shared<DrawingsRect>();

            drawing->mType             = DrawingType::Rect;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mIsOutlined       = is_outlined;
            drawing->mOutlineThickness = outline_thickness;
            drawing->mOutlineColor     = outline_color;
            drawing->mIsRounded        = is_rounded;
            drawing->mRoundingRadius   = rounding_radius;

            mDrawingRects[id] = drawing;

            return *this;
        }

        Widget& DrawText(
            const std::string&        id,
            std::shared_ptr<sf::Font> font,
            size_t                    font_size,
            sf::Vector2f              position,
            size_t                    zlevel,
            sf::Color                 fill_color,
            const std::string&        text = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType      = DrawingType::Text;
            drawing->mID        = id;
            drawing->mFont      = font;
            drawing->mFontSize  = font_size;
            drawing->mPosition  = position;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mText      = text;

            mDrawingTexts[id] = drawing;

            return *this;
        }

        Widget& DrawTexture(
            const std::string&           id,
            std::shared_ptr<sf::Texture> texture,
            sf::Vector2f                 size,
            sf::Vector2f                 position,
            size_t                       zlevel,
            sf::Color                    fill_color,
            bool                         smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType             = DrawingType::Texture;
            drawing->mTexture          = texture;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mTextureSmoothing = smoothing_enabled;

            mDrawingTextures[id] = drawing;

            return *this;
        }

        virtual void Update(const Controls& controls, sf::Vector2f pos_panel) = 0;
        virtual void Render(sf::RenderWindow& window, sf::Vector2f pos_panel) = 0;
    };

    class Canvas : public Widget {
    private:
        void RenderDrawing(sf::RenderWindow&                window,
                           const std::shared_ptr<Drawings>& drawing,
                           sf::Vector2f                     pos_widget) {
            sf::Vector2f pos_drawing = pos_widget + (drawing->mPosition);

            switch (drawing->mType) {
                case DrawingType::Rect:
                {
                    auto               rect = std::static_pointer_cast<DrawingsRect>(drawing);
                    sf::RectangleShape shape(rect->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(rect->mFillColor);

                    if (rect->mIsOutlined == true) {
                        shape.setOutlineThickness(rect->mOutlineThickness);
                        shape.setOutlineColor(rect->mOutlineColor);
                    }

                    // TODO: Implement rounded rectangles when (mIsRounded == true)

                    window.draw(shape);

                    break;
                }

                case DrawingType::Text:
                {
                    auto     text_drawing = std::static_pointer_cast<DrawingsText>(drawing);
                    sf::Text text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);

                    text.setPosition(pos_drawing);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture:
                {
                    auto               tex = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::RectangleShape shape(tex->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(tex->mFillColor);
                    shape.setTexture(tex->mTexture.get());

                    if (tex->mTextureSmoothing == true) {
                        tex->mTexture->setSmooth(true);
                    }

                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Canvas() = default;

        void Update(const Controls& controls, sf::Vector2f pos_panel) override {
            (void)controls;
            (void)pos_panel;
        }

        void Render(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f                                              pos_global = pos_panel + mPosition;
            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingRects) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }
            for (const auto& [id, drawing] : mDrawingTexts) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }
            for (const auto& [id, drawing] : mDrawingTextures) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            std::sort(all_drawings.begin(), all_drawings.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, drawing] : all_drawings) {
                RenderDrawing(window, drawing, pos_global);
            }
        }
    };

    class WidgetHandle {
    private:
        std::shared_ptr<Widget> mWidget;

    public:
        WidgetHandle(std::shared_ptr<Widget> widget) : mWidget(widget) {
        }

        std::shared_ptr<Widget> GetShared() const {
            return mWidget;
        }

        DrawingsRect& GetRect(const std::string& id) {
            return mWidget->GetRect(id);
        }

        WidgetHandle& SetSize(sf::Vector2f size) {
            mWidget->SetSize(size);

            return *this;
        }

        WidgetHandle& SetPosition(sf::Vector2f position) {
            mWidget->SetPosition(position);

            return *this;
        }

        WidgetHandle& DrawRect(const std::string& id,
                               sf::Vector2f       size,
                               sf::Vector2f       position,
                               size_t             zlevel,
                               sf::Color          fill_color,
                               bool               is_outlined       = false,
                               float              outline_thickness = 0.0f,
                               sf::Color          outline_color     = sf::Color::Black,
                               bool               is_rounded        = false,
                               float              rounding_radius   = 0.0f) {
            mWidget->DrawRect(id, size, position, zlevel, fill_color, is_outlined, outline_thickness, outline_color, is_rounded, rounding_radius);

            return *this;
        }

        WidgetHandle& DrawText(const std::string&        id,
                               std::shared_ptr<sf::Font> font,
                               size_t                    font_size,
                               sf::Vector2f              position,
                               size_t                    zlevel,
                               sf::Color                 fill_color,
                               const std::string&        text = "") {
            mWidget->DrawText(id, font, font_size, position, zlevel, fill_color, text);

            return *this;
        }

        WidgetHandle& DrawTexture(const std::string&           id,
                                  std::shared_ptr<sf::Texture> texture,
                                  sf::Vector2f                 size,
                                  sf::Vector2f                 position,
                                  size_t                       zlevel,
                                  sf::Color                    fill_color,
                                  bool                         smoothing_enabled = true) {
            mWidget->DrawTexture(id, texture, size, position, zlevel, fill_color, smoothing_enabled);

            return *this;
        }
    };

    class Panel : public std::enable_shared_from_this<Panel> {
    private:
        std::string  mName      = "Unnamed";
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::vector<std::shared_ptr<Widget>> mWidgets;

    public:
        Panel() = default;

        const std::string& GetName() const {
            return mName;
        }

        sf::Vector2f GetPosition() const {
            return mPosition;
        }

        sf::Vector2f GetSize() const {
            return mSize;
        }

        size_t GetZLevel() const {
            return mZLevel;
        }

        Panel& SetName(const std::string& name) {
            mName = name;

            return *this;
        }

        Panel& SetSize(sf::Vector2f size) {
            mSize = size;

            return *this;
        }

        Panel& SetPosition(sf::Vector2f position) {
            mPosition = position;

            return *this;
        }

        Panel& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;

            return *this;
        }

        Panel& SetVisible(bool visible) {
            mIsVisible = visible;

            return *this;
        }

        Panel& AddWidget(Widget& widget) {
            mWidgets.push_back(widget.shared_from_this());

            return *this;
        }

        inline Panel& Register(UIContext& context) {
            context.AddPanel(shared_from_this());

            return *this;
        }

        void Update(const Controls& controls) {
            if (mIsVisible == false) {
                return;
            }

            for (auto& widget : mWidgets) {
                widget->Update(controls, mPosition);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsVisible == false) {
                return;
            }

            for (auto& widget : mWidgets) {
                widget->Render(window, mPosition);
            }
        }
    };

    class UIContext {
    private:
        Controls      mControls;
        ResourceVault mResourceVault;

        std::vector<std::shared_ptr<Panel>> mPanels;

    public:
        UIContext() = default;

        ResourceVault& GetResourceVault() {
            return mResourceVault;
        }

        void ShowPanelList() const {
            std::cout << "UI Panels Listing\n";
            std::cout << "=====================\n";

            for (const auto& panel : mPanels) {
                std::cout << "Name: " << panel->GetName()
                          << "\tZ-Level: " << panel->GetZLevel() << "\n";
            }

            std::cout << std::endl;
        }

        void AddPanel(std::shared_ptr<Panel> panel) {
            mPanels.push_back(panel);
        }

        void Update(const Controls& controls) {
            mControls = controls;

            for (auto& panel : mPanels) {
                panel->Update(mControls);
            }
        }

        void Render(sf::RenderWindow& window) {
            for (auto& panel : mPanels) {
                panel->Render(window);
            }
        }
    };

    class UI {
    private:
        ResourceVault                                     mResourceVault;
        std::unordered_map<sf::RenderWindow*, UIContext*> mWindowToContext;

        static UI& GetInstance() {
            static UI instance;

            return instance;
        }

        UI() = default;

    public:
        UI(const UI&)            = delete;
        UI& operator=(const UI&) = delete;

        static void Initialize() {
            GetInstance();
        }

        static UIContext CreateContext() {
            return UIContext();
        }

        static void Bind(sf::RenderWindow& window, UIContext& context) {
            GetInstance().mWindowToContext[&window] = &context;
        }

        static std::shared_ptr<sf::Font> LoadFont(const std::string& path) {
            return GetInstance().mResourceVault.LoadFont(path);
        }

        static std::shared_ptr<sf::Texture> LoadTexture(
            const std::string& path,
            bool               srgb_enabled = false,
            const sf::IntRect& area         = sf::IntRect()) {
            return GetInstance().mResourceVault.LoadTexture(path, srgb_enabled, area);
        }

        static Panel CreatePanel() {
            return Panel();
        }

        static Canvas CreateWidget(WidgetType wtype) {
            switch (wtype) {
                case WidgetType::Canvas:
                    return Canvas();

                case WidgetType::Button:
                    throw std::runtime_error("Button widget not yet implemented");

                default:
                    throw std::runtime_error("Unknown widget type");
            }
        }

        static void ShowPanelList(sf::RenderWindow& window) {
            auto& instance = GetInstance();
            auto  iter     = instance.mWindowToContext.find(&window);

            if (iter == instance.mWindowToContext.end()) {
                throw std::runtime_error("Window not bound to any UIContext");
            }

            UIContext* context = iter->second;

            context->ShowPanelList();
        }

        static void Update(sf::RenderWindow& window) {
            auto& instance = GetInstance();
            auto  iter     = instance.mWindowToContext.find(&window);

            if (iter == instance.mWindowToContext.end()) {
                throw std::runtime_error("Window not bound to any UIContext");
            }

            UIContext* context = iter->second;

            Controls controls;
            controls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            controls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            controls.mMouse.mLPress     = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            controls.mMouse.mRPress     = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            controls.mMouse.mWPress     = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            context->Update(controls);
        }

        static void Render(sf::RenderWindow& window) {
            auto& instance = GetInstance();
            auto  iter     = instance.mWindowToContext.find(&window);

            if (iter == instance.mWindowToContext.end()) {
                throw std::runtime_error("Window not bound to any UIContext");
            }

            UIContext* context = iter->second;
            context->Render(window);
        }
    };
} // namespace Orbis