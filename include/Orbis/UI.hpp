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

    class Widget;
    class Canvas;
    class Button;
    class Panel;

    template <typename WT>
    class WidgetHandle;
    class PanelHandle;
    class UIContext;
    class UI;
} // namespace Orbis

namespace Orbis {
    class Widget : public std::enable_shared_from_this<Widget> {
    protected:
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::map<std::string, std::shared_ptr<DrawingsRect>>    mDrawingsRect;
        std::map<std::string, std::shared_ptr<DrawingsText>>    mDrawingsText;
        std::map<std::string, std::shared_ptr<DrawingsTexture>> mDrawingsTexture;

    public:
        virtual ~Widget() = default;

        DrawingsRect& GetRect(const std::string& id) {
            auto iter = mDrawingsRect.find(id);

            if (iter == mDrawingsRect.end()) {
                throw std::runtime_error("DrawingsRect with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsText& GetText(const std::string& id) {
            auto iter = mDrawingsText.find(id);

            if (iter == mDrawingsText.end()) {
                throw std::runtime_error("DrawingsText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsTexture& GetTexture(const std::string& id) {
            auto iter = mDrawingsTexture.find(id);

            if (iter == mDrawingsTexture.end()) {
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

        size_t GetZLevel() const {
            return mZLevel;
        }

        bool GetVisibility() const {
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

        Widget& SetZLevel(size_t zlevel) {
            mZLevel = zlevel;

            return *this;
        }

        Widget& SetVisibility(bool visible) {
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

            mDrawingsRect[id] = drawing;

            return *this;
        }

        Widget& DrawText(
            const std::string&        id,
            size_t                    font_size,
            sf::Vector2f              position,
            size_t                    zlevel,
            sf::Color                 fill_color,
            std::shared_ptr<sf::Font> font,
            TextAlign                 align = TextAlign::LeftTop,
            const std::string&        text  = "") {
            auto drawing = std::make_shared<DrawingsText>();

            drawing->mType      = DrawingType::Text;
            drawing->mID        = id;
            drawing->mFontSize  = font_size;
            drawing->mPosition  = position;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFont      = font;
            drawing->mAlign     = align;
            drawing->mText      = text;

            mDrawingsText[id] = drawing;

            return *this;
        }

        Widget& DrawTexture(
            const std::string&           id,
            sf::Vector2f                 size,
            sf::Vector2f                 position,
            size_t                       zlevel,
            sf::Color                    fill_color,
            std::shared_ptr<sf::Texture> texture,
            bool                         smoothing_enabled = true) {
            auto drawing = std::make_shared<DrawingsTexture>();

            drawing->mType             = DrawingType::Texture;
            drawing->mID               = id;
            drawing->mSize             = size;
            drawing->mPosition         = position;
            drawing->mZLevel           = zlevel;
            drawing->mFillColor        = fill_color;
            drawing->mTexture          = texture;
            drawing->mTextureSmoothing = smoothing_enabled;

            mDrawingsTexture[id] = drawing;

            return *this;
        }

        virtual std::shared_ptr<Widget> CloneImpl() const                                            = 0;
        virtual void                    UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) = 0;
        virtual void                    RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) = 0;
    };

    class Canvas : public Widget {
    private:
        void RenderDrawing(sf::RenderWindow&                window,
                           const std::shared_ptr<Drawings>& drawing,
                           sf::Vector2f                     pos_widget) {
            sf::Vector2f pos_drawing = pos_widget + (drawing->mPosition);

            switch (drawing->mType) {
                case DrawingType::Rect: {
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

                case DrawingType::Text: {
                    auto          text_drawing = std::static_pointer_cast<DrawingsText>(drawing);
                    sf::Text      text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);
                    sf::FloatRect bounds = text.getLocalBounds();
                    sf::Vector2f  offset = {0, 0};

                    float offset_x = 0.0f;
                    float offset_y = 0.0f;

                    if (text_drawing->mAlign == TextAlign::CenterTop ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -(bounds.size.x) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop ||
                             text_drawing->mAlign == TextAlign::RightCenter ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -(bounds.size.x);
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize)) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom ||
                             text_drawing->mAlign == TextAlign::CenterBottom ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize));
                    }

                    offset = {offset_x, offset_y};

                    text.setPosition(pos_drawing + offset);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture: {
                    auto               texture = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::RectangleShape shape(texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(texture->mFillColor);
                    shape.setTexture(texture->mTexture.get());

                    if (texture->mTextureSmoothing == true) {
                        texture->mTexture->setSmooth(true);
                    }

                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Canvas() = default;

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<Canvas>();

            cloned->mSize      = mSize;
            cloned->mPosition  = mPosition;
            cloned->mZLevel    = mZLevel;
            cloned->mIsVisible = mIsVisible;

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing       = std::make_shared<DrawingsRect>(*drawing);
                cloned->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing       = std::make_shared<DrawingsText>(*drawing);
                cloned->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing          = std::make_shared<DrawingsTexture>(*drawing);
                cloned->mDrawingsTexture[id] = cloned_drawing;
            }

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            (void)controls;
            (void)pos_panel;
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f pos_global = pos_panel + mPosition;

            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingsRect) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
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

    class Button : public Widget {
    private:
        std::function<void()> mCallback;
        ButtonState           mState = ButtonState::Normal;

        sf::Color mColorNormal  = sf::Color(200, 200, 200, 255);
        sf::Color mColorHover   = sf::Color(220, 220, 220, 255);
        sf::Color mColorPressed = sf::Color(150, 150, 150, 255);

        bool mWasPressed = false;

        void RenderDrawing(sf::RenderWindow&                window,
                           const std::shared_ptr<Drawings>& drawing,
                           sf::Vector2f                     pos_widget) {
            sf::Vector2f pos_drawing = pos_widget + drawing->mPosition;

            switch (drawing->mType) {
                case DrawingType::Rect: {
                    auto               rect = std::static_pointer_cast<DrawingsRect>(drawing);
                    sf::RectangleShape shape(rect->mSize);

                    sf::Color state_color = GetStateColor();

                    shape.setPosition(pos_drawing);
                    shape.setFillColor(state_color);

                    if (rect->mIsOutlined == true) {
                        shape.setOutlineThickness(rect->mOutlineThickness);
                        shape.setOutlineColor(rect->mOutlineColor);
                    }

                    // TODO: Implement rounded rectangles when (mIsRounded == true)

                    window.draw(shape);

                    break;
                }

                case DrawingType::Text: {
                    auto          text_drawing = std::static_pointer_cast<DrawingsText>(drawing);
                    sf::Text      text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);
                    sf::FloatRect bounds = text.getLocalBounds();
                    sf::Vector2f  offset = {0, 0};

                    float offset_x = 0.0f;
                    float offset_y = 0.0f;

                    if (text_drawing->mAlign == TextAlign::CenterTop ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -bounds.size.x / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop ||
                             text_drawing->mAlign == TextAlign::RightCenter ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -bounds.size.x;
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter ||
                        text_drawing->mAlign == TextAlign::Center ||
                        text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -static_cast<float>(text_drawing->mFontSize) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom ||
                             text_drawing->mAlign == TextAlign::CenterBottom ||
                             text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -static_cast<float>(text_drawing->mFontSize);
                    }

                    offset = {offset_x, offset_y};

                    text.setPosition(pos_drawing + offset);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }

                case DrawingType::Texture: {
                    auto               texture = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::RectangleShape shape(texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setTexture(texture->mTexture.get());

                    if (texture->mTextureSmoothing == true) {
                        texture->mTexture->setSmooth(true);
                    }

                    sf::Color blended_color = texture->mFillColor;
                    sf::Color state_color   = GetStateColor();

                    blended_color.r = (blended_color.r * state_color.r) / 255;
                    blended_color.g = (blended_color.g * state_color.g) / 255;
                    blended_color.b = (blended_color.b * state_color.b) / 255;
                    blended_color.a = (blended_color.a * state_color.a) / 255;

                    shape.setFillColor(blended_color);
                    window.draw(shape);

                    break;
                }
            }
        }

    public:
        Button() = default;

        ButtonState GetState() const {
            return mState;
        }

        sf::Color GetStateColor() const {
            switch (mState) {
                case ButtonState::Hover:
                    return mColorHover;
                case ButtonState::Pressed:
                    return mColorPressed;
                default:
                    return mColorNormal;
            }
        }

        Button& SetCallback(std::function<void()> callback) {
            mCallback = std::move(callback);

            return *this;
        }

        Button& SetStateColor(ButtonState state, sf::Color color) {
            switch (state) {
                case ButtonState::Normal: {
                    mColorNormal = color;

                    break;
                }

                case ButtonState::Hover: {
                    mColorHover = color;

                    break;
                }

                case ButtonState::Pressed: {
                    mColorPressed = color;

                    break;
                }
            }

            return *this;
        }

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<Button>();

            cloned->mSize      = mSize;
            cloned->mPosition  = mPosition;
            cloned->mZLevel    = mZLevel;
            cloned->mIsVisible = mIsVisible;

            cloned->mCallback     = mCallback;
            cloned->mColorNormal  = mColorNormal;
            cloned->mColorHover   = mColorHover;
            cloned->mColorPressed = mColorPressed;

            cloned->mState      = ButtonState::Normal;
            cloned->mWasPressed = false;

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing       = std::make_shared<DrawingsRect>(*drawing);
                cloned->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing       = std::make_shared<DrawingsText>(*drawing);
                cloned->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing          = std::make_shared<DrawingsTexture>(*drawing);
                cloned->mDrawingsTexture[id] = cloned_drawing;
            }

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f  pos_global = pos_panel + mPosition;
            sf::FloatRect bounds(pos_global, mSize);

            bool is_hovered = bounds.contains(controls.mMouse.mPosition);

            if (is_hovered == true) {
                if (controls.mMouse.mLPress == true) {
                    mState      = ButtonState::Pressed;
                    mWasPressed = true;
                }
                else {
                    if (mWasPressed == true) {
                        if (mCallback) {
                            mCallback();
                        }

                        mWasPressed = false;
                    }

                    mState = ButtonState::Hover;
                }
            }
            else {
                mState = ButtonState::Normal;

                if (controls.mMouse.mLPress == false) {
                    mWasPressed = false;
                }
            }
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f                                              pos_global = pos_panel + mPosition;
            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingsRect) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
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

        Panel& SetVisibility(bool visible) {
            mIsVisible = visible;

            return *this;
        }

        Panel& AddWidget(std::shared_ptr<Widget> widget) {
            mWidgets.push_back(widget);

            return *this;
        }

        Panel& Register(UIContext& context);

        void Update(const Controls& controls) {
            if (mIsVisible == false) {
                return;
            }

            for (auto& widget : mWidgets) {
                widget->UpdateImpl(controls, mPosition);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsVisible == false) {
                return;
            }

            for (auto& widget : mWidgets) {
                widget->RenderImpl(window, mPosition);
            }
        }
    };

    template <typename WT>
    class WidgetHandle {
    private:
        std::shared_ptr<WT> mWidget;

    public:
        WidgetHandle(std::shared_ptr<WT> widget) : mWidget(widget) {};

        std::shared_ptr<Widget> GetShared() const {
            return std::static_pointer_cast<Widget>(mWidget);
        }

        DrawingsRect& GetRect(const std::string& id) {
            return mWidget->GetRect(id);
        }

        DrawingsText& GetText(const std::string& id) {
            return mWidget->GetText(id);
        }

        DrawingsTexture& GetTexture(const std::string& id) {
            return mWidget->GetTexture(id);
        }

        WidgetHandle& SetSize(sf::Vector2f size) {
            mWidget->SetSize(size);

            return *this;
        }

        WidgetHandle& SetPosition(sf::Vector2f position) {
            mWidget->SetPosition(position);

            return *this;
        }

        WidgetHandle& SetZLevel(size_t zlevel) {
            mWidget->SetZLevel(zlevel);

            return *this;
        }

        WidgetHandle& SetVisibility(bool visible) {
            mWidget->SetVisibility(visible);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Button>, WidgetHandle&> SetCallback(std::function<void()> callback) {
            static_cast<Button*>(mWidget.get())->SetCallback(callback);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Button>, WidgetHandle&> SetStateColor(ButtonState state, sf::Color color) {
            static_cast<Button*>(mWidget.get())->SetStateColor(state, color);

            return *this;
        }

        WidgetHandle& DrawRect(
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
            mWidget->DrawRect(id, size, position, zlevel, fill_color, is_outlined, outline_thickness, outline_color, is_rounded, rounding_radius);

            return *this;
        }

        WidgetHandle& DrawText(
            const std::string&        id,
            size_t                    font_size,
            sf::Vector2f              position,
            size_t                    zlevel,
            sf::Color                 fill_color,
            std::shared_ptr<sf::Font> font,
            TextAlign                 align = TextAlign::LeftTop,
            const std::string&        text  = "") {
            mWidget->DrawText(id, font_size, position, zlevel, fill_color, font, align, text);

            return *this;
        }

        WidgetHandle& DrawTexture(
            const std::string&           id,
            sf::Vector2f                 size,
            sf::Vector2f                 position,
            size_t                       zlevel,
            sf::Color                    fill_color,
            std::shared_ptr<sf::Texture> texture,
            bool                         smoothing_enabled = true) {
            mWidget->DrawTexture(id, size, position, zlevel, fill_color, texture, smoothing_enabled);

            return *this;
        }

        WidgetHandle Clone() const {
            auto cloned = std::static_pointer_cast<WT>(mWidget->CloneImpl());

            return WidgetHandle<WT>(cloned);
        }
    };

    class PanelHandle {
    private:
        std::shared_ptr<Panel> mPanel;

    public:
        PanelHandle(std::shared_ptr<Panel> panel) : mPanel(panel) {};

        std::shared_ptr<Panel> GetShared() const {
            return mPanel;
        }

        PanelHandle& SetName(const std::string& name) {
            mPanel->SetName(name);

            return *this;
        }

        PanelHandle& SetSize(sf::Vector2f size) {
            mPanel->SetSize(size);

            return *this;
        }

        PanelHandle& SetPosition(sf::Vector2f position) {
            mPanel->SetPosition(position);

            return *this;
        }

        PanelHandle& SetZLevel(size_t zlevel) {
            mPanel->SetZLevel(zlevel);

            return *this;
        }

        PanelHandle& SetVisibility(bool visible) {
            mPanel->SetVisibility(visible);

            return *this;
        }

        template <typename WT>
        PanelHandle& AddWidget(const WidgetHandle<WT>& widget_handle) {
            mPanel->AddWidget(widget_handle.GetShared());

            return *this;
        }

        PanelHandle& Register(UIContext& context);
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
                std::cout
                    << "Name: " << panel->GetName() << "\t"
                    << "ZLevel: " << panel->GetZLevel() << "\n";
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

        static UIContext CreateContext() {
            return UIContext();
        }

        static PanelHandle CreatePanel() {
            return PanelHandle(std::make_shared<Panel>());
        }

        template <WidgetType Type>
        static auto CreateWidget() {
            if constexpr (Type == WidgetType::Canvas) {
                return WidgetHandle<Canvas>(std::make_shared<Canvas>());
            }
            else if constexpr (Type == WidgetType::Button) {
                return WidgetHandle<Button>(std::make_shared<Button>());
            }
            else {
                static_assert(Type == WidgetType::Canvas || Type == WidgetType::Button, "Unknown widget type");
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

    inline Panel& Panel::Register(UIContext& context) {
        context.AddPanel(shared_from_this());

        return *this;
    }

    inline PanelHandle& PanelHandle::Register(UIContext& context) {
        mPanel->Register(context);

        return *this;
    }
} // namespace Orbis