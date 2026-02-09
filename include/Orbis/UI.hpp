#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/System/Controls.hpp"
#include "Orbis/System/ResourceVault.hpp"
#include "Orbis/Widgets/Button.hpp"
#include "Orbis/Widgets/Canvas.hpp"
#include "Orbis/Widgets/Slider.hpp"
#include "Orbis/Widgets/Textbox.hpp"
#include "Orbis/Widgets/Widget.hpp"

namespace Orbis {
    class Widget;
    class Panel;
    class Scene;

    template <typename WT>
    class WidgetHandle;
    class PanelHandle;
    class SceneHandle;
    class UIContext;
    class UI;
} // namespace Orbis

namespace Orbis {
    class Panel : public std::enable_shared_from_this<Panel> {
    private:
        std::string  mName      = "Panel_Unnamed";
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::vector<std::shared_ptr<Widget>> mWidgets;
        std::optional<AnimationState>        mPosAnimation;

        void UpdateAnimation() {
            if (mPosAnimation.has_value() == true) {
                if (mPosAnimation->IsComplete() == true) {
                    mPosition = mPosAnimation->mTargetPos;

                    if (mPosAnimation->mOnComplete) {
                        mPosAnimation->mOnComplete();
                    }

                    mPosAnimation.reset();
                }
                else {
                    mPosition = mPosAnimation->GetCurrentPosition();
                }
            }
        }

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

        bool GetVisibility() const {
            return mIsVisible;
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

        Panel& PositionAnimation(sf::Vector2f target, float duration, std::function<void()> on_complete = nullptr, std::function<float(float)> easing = Anim::EaseOutQuad) {
            if (mPosAnimation.has_value() == false) {
                mPosAnimation = AnimationState();
            }

            mPosAnimation->Start(mPosition, target, duration, std::move(on_complete));
            mPosAnimation->SetEasing(easing);

            return *this;
        }

        void Update(const Controls& controls) {
            UpdateAnimation();

            if (mIsVisible == false) {
                return;
            }

            std::vector<std::pair<size_t, std::shared_ptr<Widget>>> sorted_widgets;

            for (auto& widget : mWidgets) {
                sorted_widgets.push_back({widget->GetZLevel(), widget});
            }

            std::sort(sorted_widgets.begin(), sorted_widgets.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, widget] : sorted_widgets) {
                widget->UpdateImpl(controls, mPosition);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsVisible == false) {
                return;
            }

            std::vector<std::pair<size_t, std::shared_ptr<Widget>>> sorted_widgets;

            for (auto& widget : mWidgets) {
                sorted_widgets.push_back({widget->GetZLevel(), widget});
            }

            std::sort(sorted_widgets.begin(), sorted_widgets.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, widget] : sorted_widgets) {
                widget->RenderImpl(window, mPosition);
            }
        }
    };

    class Scene : public std::enable_shared_from_this<Scene> {
    private:
        std::string                         mName     = "Scene_Unnamed";
        bool                                mIsActive = false;
        std::vector<std::shared_ptr<Panel>> mPanels;
        bool                                mIsRegistered = false;

    public:
        Scene() = default;

        const std::string& GetName() const {
            return mName;
        }

        bool IsActive() const {
            return mIsActive;
        }

        Scene& SetName(const std::string& name) {
            mName = name;
            return *this;
        }

        Scene& SetActive(bool is_active) {
            mIsActive = is_active;

            for (auto& panel : mPanels) {
                panel->SetVisibility(is_active);
            }

            return *this;
        }

        Scene& AddPanel(std::shared_ptr<Panel> panel) {
            mPanels.push_back(panel);

            return *this;
        }

        void Update(const Controls& controls) {
            if (mIsActive == false) {
                return;
            }

            std::vector<std::pair<size_t, std::shared_ptr<Panel>>> sorted_panels;

            for (auto& panel : mPanels) {
                sorted_panels.push_back({panel->GetZLevel(), panel});
            }

            std::sort(sorted_panels.begin(), sorted_panels.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, panel] : sorted_panels) {
                panel->Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsActive == false) {
                return;
            }

            std::vector<std::pair<size_t, std::shared_ptr<Panel>>> sorted_panels;

            for (auto& panel : mPanels) {
                sorted_panels.push_back({panel->GetZLevel(), panel});
            }

            std::sort(sorted_panels.begin(), sorted_panels.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, panel] : sorted_panels) {
                panel->Render(window);
            }
        }

        Scene& Register(UIContext& context);
    };

    /*
    // API Handlers (Widget, Panel, Scene)
    */
    template <typename T>
    concept IsCanvas = std::is_same_v<T, Canvas>;

    template <typename T>
    concept IsButton = std::is_same_v<T, Button>;

    template <typename T>
    concept IsSlider = std::is_same_v<T, Slider>;

    template <typename T>
    concept IsTextboxSingle = std::is_same_v<T, TextboxSingle>;

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

        DrawingsWText& GetWText(const std::string& id) {
            return mWidget->GetWText(id);
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

        WidgetHandle& PositionAnimation(sf::Vector2f target, float duration, std::function<void()> on_complete = nullptr, std::function<float(float)> easing = Anim::EaseOutQuad) {
            mWidget->PositionAnimation(target, duration, on_complete, easing);

            return *this;
        }

        WidgetHandle& ScaleAnimation(sf::Vector2f target_scale, float duration, std::function<void()> on_complete = nullptr, std::function<float(float)> easing = Anim::EaseOutQuad) {
            mWidget->ScaleAnimation(target_scale, duration, on_complete, easing);

            return *this;
        }

        WidgetHandle& CancelAnimation() {
            mWidget->CancelAnimation();

            return *this;
        }

        // Button
        WidgetHandle& SetOnButtonPressed(std::function<void()> callback) requires IsButton<WT> {
            static_cast<Button*>(mWidget.get())->SetOnButtonPressed(callback);

            return *this;
        }

        WidgetHandle& SetStateColor(ButtonState state, sf::Color color) requires IsButton<WT> {
            static_cast<Button*>(mWidget.get())->SetStateColor(state, color);

            return *this;
        }

        // Slider
        float GetValue() const requires IsSlider<WT> {
            return static_cast<const Slider*>(mWidget.get())->GetValue();
        }

        WidgetHandle& SetRange(float min, float max) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetRange(min, max);

            return *this;
        }

        WidgetHandle& SetValue(float value) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetValue(value);

            return *this;
        }

        WidgetHandle& SetStepSize(float step) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetStepSize(step);

            return *this;
        }

        WidgetHandle& SetOrientation(bool horizontal) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetOrientation(horizontal);

            return *this;
        }

        WidgetHandle& SetOnValueChanged(std::function<void(float)> callback) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetOnValueChanged(callback);

            return *this;
        }

        WidgetHandle& SetTrackSize(sf::Vector2f size) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetTrackSize(size);

            return *this;
        }

        WidgetHandle& SetTrackColor(sf::Color color) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetTrackColor(color);

            return *this;
        }

        WidgetHandle& SetFillColor(sf::Color color) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetFillColor(color);

            return *this;
        }

        WidgetHandle& SetHandleSize(sf::Vector2f size) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetHandleSize(size);

            return *this;
        }

        WidgetHandle& SetHandleColor(SliderState state, sf::Color color) requires IsSlider<WT> {
            static_cast<Slider*>(mWidget.get())->SetHandleColor(state, color);

            return *this;
        }

        // TextboxSingle
        bool IsContentEmpty() const requires IsTextboxSingle<WT> {
            return static_cast<const TextboxSingle*>(mWidget.get())->IsContentEmpty();
        }

        const sf::String& GetTextContent() const requires IsTextboxSingle<WT> {
            return static_cast<const TextboxSingle*>(mWidget.get())->GetTextContent();
        }

        WidgetHandle& SetPlaceholder(std::string placeholder) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetPlaceholder(placeholder);

            return *this;
        }

        WidgetHandle& SetPlaceholderW(std::wstring placeholder_w) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetPlaceholder(placeholder_w);

            return *this;
        }

        WidgetHandle& SetEditableText(const std::string& text_id) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetEditableText(text_id);

            return *this;
        }

        WidgetHandle& SetEditableWText(const std::string& wtext_id) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetEditableWText(wtext_id);

            return *this;
        }

        WidgetHandle& SetPadding(float padding) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetPadding(padding);

            return *this;
        }

        WidgetHandle& SetText(const sf::String& text) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetText(text);

            return *this;
        }

        WidgetHandle& ClearText() requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->ClearText();

            return *this;
        }

        WidgetHandle& SetOnTextChanged(std::function<void(const sf::String&)> callback) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetOnTextChanged(callback);

            return *this;
        }

        WidgetHandle& SetOnEnterPressed(std::function<void()> callback) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->SetOnEnterPressed(callback);

            return *this;
        }

        WidgetHandle& BindInt(int* value_ptr, int min_value = INT_MIN, int max_value = INT_MAX) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->BindInt(value_ptr, min_value, max_value);

            return *this;
        }

        WidgetHandle& BindFloat(float* value_ptr, float min_value = -FLT_MAX, float max_value = FLT_MAX) requires IsTextboxSingle<WT> {
            static_cast<TextboxSingle*>(mWidget.get())->BindFloat(value_ptr, min_value, max_value);

            return *this;
        }

        // Drawings
        WidgetHandle& DrawLine(const std::string& id, const std::vector<sf::Vector2f>& points, size_t zlevel, sf::Color color, float thickness) {
            mWidget->DrawLine(id, points, zlevel, color, thickness);

            return *this;
        }

        WidgetHandle& DrawRect(const std::string& id, sf::Vector2f size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, bool is_outlined = false, float outline_thickness = 0.0f, sf::Color outline_color = sf::Color::Black, bool is_rounded = false, float rounding_radius = 0.0f) {
            mWidget->DrawRect(id, size, position, zlevel, fill_color, is_outlined, outline_thickness, outline_color, is_rounded, rounding_radius);

            return *this;
        }

        WidgetHandle& DrawText(const std::string& id, size_t font_size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Font> font, TextAlign align = TextAlign::LeftTop, const std::string& text = "") {
            mWidget->DrawText(id, font_size, position, zlevel, fill_color, font, align, text);

            return *this;
        }

        WidgetHandle& DrawWText(const std::string& id, size_t font_size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Font> font, TextAlign align = TextAlign::LeftTop, const std::wstring& wtext = L"") {
            mWidget->DrawWText(id, font_size, position, zlevel, fill_color, font, align, wtext);

            return *this;
        }

        WidgetHandle& DrawTexture(const std::string& id, sf::Vector2f size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Texture> texture, sf::Vector2f scale = {1.0f, 1.0f}) {
            mWidget->DrawTexture(id, size, position, zlevel, fill_color, texture, scale);

            return *this;
        }

        // Implementations
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

        PanelHandle& PositionAnimation(sf::Vector2f target, float duration, std::function<void()> on_complete = nullptr, std::function<float(float)> easing = Anim::EaseOutQuad) {
            mPanel->PositionAnimation(target, duration, std::move(on_complete), easing);

            return *this;
        }
    };

    class SceneHandle {
    private:
        std::shared_ptr<Scene> mScene;

    public:
        SceneHandle(std::shared_ptr<Scene> scene) : mScene(scene) {};

        std::shared_ptr<Scene> GetShared() const {
            return mScene;
        }

        SceneHandle& SetName(const std::string& name) {
            mScene->SetName(name);

            return *this;
        }

        SceneHandle& SetActive(bool is_active) {
            mScene->SetActive(is_active);

            return *this;
        }

        SceneHandle& AddPanel(const PanelHandle& panel_handle) {
            mScene->AddPanel(panel_handle.GetShared());

            return *this;
        }

        SceneHandle& Register(UIContext& context);
    };

    class UIContext {
    private:
        Controls mControls;

        std::vector<std::shared_ptr<Panel>> mPanels; // Standalone panels?
        std::vector<std::shared_ptr<Scene>> mScenes;

    public:
        UIContext() = default;

        void ShowPanelList() const {
            std::cout << "UI Panels Listing\n";
            std::cout << "=====================\n";

            for (const auto& panel : mPanels) {
                std::cout
                    << "Name: " << panel->GetName() << "\t"
                    << "Visible: " << panel->GetVisibility() << "\t"
                    << "ZLevel: " << panel->GetZLevel() << "\n";
            }

            std::cout << std::endl;
        }

        void ShowSceneList() const {
            std::cout << "UI Scenes Listing\n";
            std::cout << "=====================\n";

            for (const auto& scene : mScenes) {
                std::cout
                    << "Name: " << scene->GetName() << "\t"
                    << "Active: " << (scene->IsActive() ? "Yes" : "No") << "\n";
            }

            std::cout << std::endl;
        }

        void AddPanel(std::shared_ptr<Panel> panel) {
            mPanels.push_back(panel);
        }

        void AddScene(std::shared_ptr<Scene> scene) {
            mScenes.push_back(scene);
        }

        void Update(const Controls& controls) {
            mControls = controls;

            for (auto& panel : mPanels) {
                panel->Update(mControls);
            }

            for (auto& scene : mScenes) {
                scene->Update(mControls);
            }
        }

        void Render(sf::RenderWindow& window) {
            for (auto& panel : mPanels) {
                panel->Render(window);
            }

            for (auto& scene : mScenes) {
                scene->Render(window);
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

        static std::shared_ptr<sf::Texture> LoadTexture(const std::string& path, bool smoothing_enabled = false, bool srgb_enabled = false, const sf::IntRect& area = sf::IntRect()) {
            return GetInstance().mResourceVault.LoadTexture(path, smoothing_enabled, srgb_enabled, area);
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
            else if constexpr (Type == WidgetType::TextboxSingle) {
                return WidgetHandle<TextboxSingle>(std::make_shared<TextboxSingle>());
            }
            else {
                static_assert(Type == WidgetType::Canvas || Type == WidgetType::Button || Type == WidgetType::Slider || Type == WidgetType::TextboxSingle, "Unknown widget type");
            }
        }

        static SceneHandle CreateScene() {
            return SceneHandle(std::make_shared<Scene>());
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

        static void ShowSceneList(sf::RenderWindow& window) {
            auto& instance = GetInstance();
            auto  iter     = instance.mWindowToContext.find(&window);

            if (iter == instance.mWindowToContext.end()) {
                throw std::runtime_error("Window not bound to any UIContext");
            }

            UIContext* context = iter->second;

            context->ShowSceneList();
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
                // Backspace=8, Tab=9, Enter=13, Delete=127
                if (32 <= text_entered->unicode && text_entered->unicode != 127) {
                    keyboard.mTextEntered += text_entered->unicode;
                }
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

            controls.mMouse             = instance.mMouseBuffers[&window];
            controls.mKeyboard          = instance.mKeyboardBuffers[&window];
            controls.mMouse.mPosition.x = static_cast<float>(sf::Mouse::getPosition(window).x);
            controls.mMouse.mPosition.y = static_cast<float>(sf::Mouse::getPosition(window).y);

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

    inline Scene& Scene::Register(UIContext& context) {
        if (mIsRegistered == true) {
            throw std::runtime_error("Scene already registered to a context. unlike Panels, Scenes are not shareable across multiple contexts.");
        }

        context.AddScene(shared_from_this());

        mIsRegistered = true;

        return *this;
    }

    inline SceneHandle& SceneHandle::Register(UIContext& context) {
        mScene->Register(context);

        return *this;
    }
} // namespace Orbis