#include <Orbis/UI.hpp>
#include <Orbis/Utils.hpp>

using namespace Orbis;

int main() {
    sf::Vector2f     screen_size({800, 800});
    sf::RenderWindow window(sf::VideoMode({static_cast<uint32_t>(screen_size.x), static_cast<uint32_t>(screen_size.y)}), "Graph Test", sf::Style::Default);

    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false);

    UI::Initialize();

    UIContext context = UI::CreateContext();
    UI::Bind(window, context);

    auto font = UI::LoadFont("./res/roboto.ttf");

    auto  points          = Utils::GeneratePoints([](float x) { return (x * x * x) + (x * x) - (3.0f * x) + 4.0f; }, -10.0f, 10.0f, 0.05f, 40.0f, 40.0f, {400, 400});
    float screen_center_x = screen_size.x / 2.0f;
    float screen_center_y = screen_size.y / 2.0f;

    float        dot_position = 0.5f;
    sf::Vector2f dot_coords   = {400, 400};

    float        x_min   = -10.0f;
    float        x_max   = 10.0f;
    float        scale_x = 40.0f;
    float        scale_y = 40.0f;
    sf::Vector2f origin  = {400, 400};

    auto target = [](float x) -> float {
        return (x * x * x) + (x * x) - (3.0f * x) + 4.0f;
    };

    auto target_d = [](float x) -> float {
        return (3.0f * x * x) + (2.0f * x) - 3.0f;
    };

    auto get_actual_x = [x_min, x_max](float t) -> float {
        return x_min + t * (x_max - x_min);
    };

    auto get_curve_position = [&points](float t) -> sf::Vector2f {
        if (points.empty() == true) {
            return {0, 0};
        }

        t = std::max(0.0f, std::min(1.0f, t));

        float  index_float = t * static_cast<float>(points.size() - 1);
        size_t index_low   = static_cast<size_t>(std::floor(index_float));
        size_t index_high  = static_cast<size_t>(std::ceil(index_float));

        index_low  = std::min(index_low, points.size() - 1);
        index_high = std::min(index_high, points.size() - 1);

        if (index_low == index_high) {
            return points[index_low];
        }

        float        local_t = index_float - static_cast<float>(index_low);
        sf::Vector2f p1      = points[index_low];
        sf::Vector2f p2      = points[index_high];

        return {
            p1.x + local_t * (p2.x - p1.x),
            p1.y + local_t * (p2.y - p1.y)};
    };

    dot_coords = get_curve_position(dot_position);

    auto canvas_field    = UI::CreateWidget<WidgetType::Canvas>();
    auto canvas_dot      = UI::CreateWidget<WidgetType::Canvas>();
    auto canvas_tan      = UI::CreateWidget<WidgetType::Canvas>();
    auto canvas_label    = UI::CreateWidget<WidgetType::Canvas>();
    auto slider_position = UI::CreateWidget<WidgetType::Slider>();

    canvas_field
        .SetSize({800, 800})
        .SetPosition({0, 0})
        .DrawRect("background", {800, 800}, {0, 0}, 0, sf::Color(30, 30, 30, 255))
        .DrawRect("axis_x", {screen_size.x, 3.0f}, {0.0f, screen_center_y - 1.5f}, 2, sf::Color(150, 150, 150, 255))
        .DrawRect("axis_y", {3.0f, screen_size.y}, {screen_center_x - 1.5f, 0.0f}, 2, sf::Color(150, 150, 150, 255));

    for (int i = 1; i <= 10; ++i) {
        float position = static_cast<float>(i) * 80.0f;

        canvas_field
            .DrawRect("hline_" + std::to_string(i), {screen_size.x, 2.0f}, {0.0f, position - 1.0f}, 1, sf::Color(50, 50, 50, 255))
            .DrawRect("vline_" + std::to_string(i), {2.0f, screen_size.y}, {position - 1.0f, 0.0f}, 1, sf::Color(50, 50, 50, 255));
    }

    canvas_dot
        .SetSize({12, 12})
        .SetPosition({dot_coords.x - 6, dot_coords.y - 6})
        .DrawRect("dot", {12, 12}, {0, 0}, 10, sf::Color(255, 100, 100, 255), true, 2.0f, sf::Color::White, true, 6.0f);

    canvas_tan
        .SetSize({800, 800})
        .SetPosition({0, 0});

    canvas_label
        .SetSize({200, 30})
        .SetPosition({10, 20})
        .DrawText("label_pos", 14, {0, 15}, 0, sf::Color::White, font, TextAlign::LeftCenter, "Position: 0.500")
        .DrawText("label_slope", 14, {0, 30}, 0, sf::Color::White, font, TextAlign::LeftCenter, "Slope: 0.000");

    slider_position
        .SetSize({400, 20})
        .SetPosition({200, 20})
        .SetRange(0.0f, 1.0f)
        .SetValue(dot_position)
        .SetStepSize(0.001f)
        .SetTrackSize({400, 4})
        .SetTrackColor(sf::Color(100, 100, 100, 255))
        .SetFillColor(sf::Color(100, 150, 255, 255))
        .SetHandleSize({16, 16})
        .SetHandleColor(SliderState::Normal, sf::Color(200, 200, 200, 255))
        .SetHandleColor(SliderState::Hover, sf::Color(220, 220, 220, 255))
        .SetHandleColor(SliderState::Dragging, sf::Color(150, 150, 255, 255))
        .SetCallback([&dot_position, &dot_coords, &get_curve_position](float value) {
            dot_position = value;
            dot_coords   = get_curve_position(value);
        });

    auto panel_main = UI::CreatePanel();

    panel_main
        .SetName("GraphExample")
        .SetSize({800, 800})
        .SetPosition({0, 0})
        .SetZLevel(0)
        .AddWidget(canvas_field)
        .AddWidget(canvas_dot)
        .AddWidget(canvas_tan)
        .AddWidget(canvas_label)
        .AddWidget(slider_position)
        .Register(context);

    auto& label_text  = canvas_label.GetText("label_pos");
    auto& label_slope = canvas_label.GetText("label_slope");

    while (window.isOpen()) {
        UI::Update(window);

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() == true) {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) == true) {
                window.close();
            }
        }

        float x_current      = get_actual_x(dot_position);
        float y_current      = target(x_current);
        float slope          = target_d(x_current);
        float tangent_length = 2.0f;

        float x1 = x_current - tangent_length;
        float x2 = x_current + tangent_length;
        float y1 = y_current + slope * (x1 - x_current);
        float y2 = y_current + slope * (x2 - x_current);

        sf::Vector2f              tan_p1 = {origin.x + (x1 * scale_x), origin.y - (y1 * scale_y)};
        sf::Vector2f              tan_p2 = {origin.x + (x2 * scale_x), origin.y - (y2 * scale_y)};
        std::vector<sf::Vector2f> tan_ps = {tan_p1, tan_p2};

        canvas_dot.SetPosition({dot_coords.x - 6, dot_coords.y - 6});

        window.clear();

        canvas_field.DrawLine("graph_curve", points, 3, sf::Color(200, 100, 100, 255), 4.0f);
        canvas_tan.DrawLine("tangent_line", tan_ps, 5, sf::Color(100, 255, 100, 255), 3.0f);

        char buffer[64];

        std::snprintf(buffer, sizeof(buffer), "Position: %.3f", dot_position);

        label_text.mText = buffer;

        std::snprintf(buffer, sizeof(buffer), "Slope: %.3f", slope);

        label_slope.mText = buffer;

        UI::Render(window);

        window.display();
    }

    return 0;
}