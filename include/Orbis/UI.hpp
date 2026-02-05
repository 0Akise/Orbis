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
    class Panel;
    class Scene;

    class PanelHandle;
    class SceneHandle;
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

    class Scene : public std::enable_shared_from_this<Scene> {
    private:
        std::string                         mName     = "Unnamed";
        bool                                mIsActive = true;
        std::vector<std::shared_ptr<Panel>> mPanels;
        bool                                mIsRegistered = false;

    public:
        Scene& SetName(const std::string& name) {
            mName = name;
            return *this;
        }

        Scene& SetActive(bool active) {
            mIsActive = active;

            for (auto& panel : mPanels) {
                panel->SetVisibility(active);
            }

            return *this;
        }

        Scene& AddPanel(std::shared_ptr<Panel> panel) {
            mPanels.push_back(panel);
            return *this;
        }

        Scene& Register(UIContext& context);

        void Update(const Controls& controls) {
            if (mIsActive == false) {
                return;
            }

            for (auto& panel : mPanels) {
                panel->Update(controls);
            }
        }

        void Render(sf::RenderWindow& window) {
            if (mIsActive == false) {
                return;
            }
            for (auto& panel : mPanels) {
                panel->Render(window);
            }
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
        Controls mControls;

        std::vector<std::shared_ptr<Panel>> mPanels;

    public:
        UIContext() = default;

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

        static std::shared_ptr<sf::Texture> LoadTexture(const std::string& path, bool srgb_enabled = false, const sf::IntRect& area = sf::IntRect()) {
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
            else if constexpr (Type == WidgetType::TextboxSingle) {
                return WidgetHandle<TextboxSingle>(std::make_shared<TextboxSingle>());
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

    inline Panel& Panel::Register(UIContext& context) {
        context.AddPanel(shared_from_this());

        return *this;
    }

    inline PanelHandle& PanelHandle::Register(UIContext& context) {
        mPanel->Register(context);

        return *this;
    }

    inline Scene& Scene::Register(UIContext& context) {
        if (mIsRegistered == true) {
            throw std::runtime_error("Scene already registered to a context. unlike Panels, Scenes are not shareable across multiple contexts.");
        }

        context.AddScene(shared_from_this()); // TODO

        mIsRegistered = true;

        return *this;
    }
} // namespace Orbis