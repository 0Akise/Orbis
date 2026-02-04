#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/System/Controls.hpp"
#include "Orbis/System/ResourceVault.hpp"
#include "Orbis/UI/Button.hpp"
#include "Orbis/UI/Canvas.hpp"
#include "Orbis/UI/Slider.hpp"
#include "Orbis/UI/Textbox.hpp"
#include "Orbis/UI/Widget.hpp"

namespace Orbis {
    class Panel;
    class Scene;

    template <typename WT>
    class WidgetHandle;
    class PanelHandle;
    class UIContext;
    class UI;
} // namespace Orbis

namespace Orbis {
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

    class Scene {
        // TODO
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

        // Button
        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Button>, WidgetHandle&> SetOnButtonPressed(std::function<void()> callback) {
            static_cast<Button*>(mWidget.get())->SetOnButtonPressed(callback);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Button>, WidgetHandle&> SetStateColor(ButtonState state, sf::Color color) {
            static_cast<Button*>(mWidget.get())->SetStateColor(state, color);

            return *this;
        }

        // Slider
        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, float> GetValue() const {
            return static_cast<const Slider*>(mWidget.get())->GetValue();
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetRange(float min, float max) {
            static_cast<Slider*>(mWidget.get())->SetRange(min, max);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetValue(float value) {
            static_cast<Slider*>(mWidget.get())->SetValue(value);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetStepSize(float step) {
            static_cast<Slider*>(mWidget.get())->SetStepSize(step);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetOrientation(bool horizontal) {
            static_cast<Slider*>(mWidget.get())->SetOrientation(horizontal);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetOnValueChanged(std::function<void(float)> callback) {
            static_cast<Slider*>(mWidget.get())->SetOnValueChanged(callback);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetTrackSize(sf::Vector2f size) {
            static_cast<Slider*>(mWidget.get())->SetTrackSize(size);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetTrackColor(sf::Color color) {
            static_cast<Slider*>(mWidget.get())->SetTrackColor(color);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetFillColor(sf::Color color) {
            static_cast<Slider*>(mWidget.get())->SetFillColor(color);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetHandleSize(sf::Vector2f size) {
            static_cast<Slider*>(mWidget.get())->SetHandleSize(size);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, Slider>, WidgetHandle&> SetHandleColor(SliderState state, sf::Color color) {
            static_cast<Slider*>(mWidget.get())->SetHandleColor(state, color);

            return *this;
        }

        // TextboxSingle
        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, TextboxSingle>, WidgetHandle&> SetOnTextChanged(std::function<void(const sf::String&)> callback) {
            static_cast<TextboxSingle*>(mWidget.get())->SetOnTextChanged(callback);

            return *this;
        }

        template <typename U = WT>
        std::enable_if_t<std::is_same_v<U, TextboxSingle>, WidgetHandle&> SetOnEnterPressed(std::function<void()> callback) {
            static_cast<TextboxSingle*>(mWidget.get())->SetOnEnterPressed(callback);

            return *this;
        }

        WidgetHandle& DrawLine(
            const std::string&               id,
            const std::vector<sf::Vector2f>& points,
            size_t                           zlevel,
            sf::Color                        color,
            float                            thickness) {
            mWidget->DrawLine(id, points, zlevel, color, thickness);

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
        std::unordered_map<sf::RenderWindow*, Mouse>      mMouseBuffers;
        std::unordered_map<sf::RenderWindow*, Keyboard>   mKeyboardBuffers;

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
            else if constexpr (Type == WidgetType::Slider) {
                return WidgetHandle<Slider>(std::make_shared<Slider>());
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

        static void ProcessEvent(sf::RenderWindow& window, const sf::Event& event) {
            auto& instance = GetInstance();

            auto iter_kb = instance.mKeyboardBuffers.find(&window);

            if (iter_kb == instance.mKeyboardBuffers.end()) {
                instance.mKeyboardBuffers[&window] = Keyboard();

                iter_kb = instance.mKeyboardBuffers.find(&window);
            }

            Keyboard& keyboard = iter_kb->second;

            if (const auto* text_entered = event.getIf<sf::Event::TextEntered>()) {
                keyboard.mTextEntered += text_entered->unicode;
            }
            else if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
                keyboard.mKeysPressed.push_back(key_pressed->code);

                keyboard.mIsCPressed = key_pressed->control;
                keyboard.mIsSPressed = key_pressed->shift;
                keyboard.mIsAPressed = key_pressed->alt;
            }
            else if (const auto* key_released = event.getIf<sf::Event::KeyReleased>()) {
                keyboard.mKeysReleased.push_back(key_released->code);

                keyboard.mIsCPressed = key_released->control;
                keyboard.mIsSPressed = key_released->shift;
                keyboard.mIsAPressed = key_released->alt;
            }

            auto iter_m = instance.mMouseBuffers.find(&window);

            if (iter_m == instance.mMouseBuffers.end()) {
                instance.mMouseBuffers[&window] = Mouse();

                iter_m = instance.mMouseBuffers.find(&window);
            }

            Mouse& mouse = iter_m->second;

            if (const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
                mouse.mButtonsPressed.push_back(mouse_pressed->button);

                switch (mouse_pressed->button) {
                    case sf::Mouse::Button::Left: {
                        mouse.mIsLPressed = true;
                        break;
                    }
                    case sf::Mouse::Button::Right: {
                        mouse.mIsRPressed = true;
                        break;
                    }
                    case sf::Mouse::Button::Middle: {
                        mouse.mIsWPressed = true;
                        break;
                    }
                    case sf::Mouse::Button::Extra1: {
                        mouse.mIsE1Pressed = true;
                        break;
                    }
                    case sf::Mouse::Button::Extra2: {
                        mouse.mIsE2Pressed = true;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
            else if (const auto* mouse_released = event.getIf<sf::Event::MouseButtonReleased>()) {
                mouse.mButtonsReleased.push_back(mouse_released->button);

                switch (mouse_released->button) {
                    case sf::Mouse::Button::Left: {
                        mouse.mIsLPressed = false;
                        break;
                    }
                    case sf::Mouse::Button::Right: {
                        mouse.mIsRPressed = false;
                        break;
                    }
                    case sf::Mouse::Button::Middle: {
                        mouse.mIsWPressed = false;
                        break;
                    }
                    case sf::Mouse::Button::Extra1: {
                        mouse.mIsE1Pressed = false;
                        break;
                    }
                    case sf::Mouse::Button::Extra2: {
                        mouse.mIsE2Pressed = false;
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        static void Update(sf::RenderWindow& window) {
            auto& instance = GetInstance();
            auto  iter     = instance.mWindowToContext.find(&window);

            if (iter == instance.mWindowToContext.end()) {
                throw std::runtime_error("Window not bound to any UIContext");
            }

            UIContext* context = iter->second;
            Controls   controls;

            controls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            controls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);
            controls.mMouse             = instance.mMouseBuffers[&window];
            controls.mKeyboard          = instance.mKeyboardBuffers[&window];

            instance.mMouseBuffers[&window].ClearFrameEvents();
            instance.mKeyboardBuffers[&window].ClearFrameEvents();

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