#pragma once;

namespace Orbis {
    enum class DrawingType {
        Line,
        Rect,
        Text,
        Texture,
    };

    enum class TextAlign {
        LeftTop,
        CenterTop,
        RightTop,
        LeftCenter,
        Center,
        RightCenter,
        LeftBottom,
        CenterBottom,
        RightBottom,
    };

    enum class WidgetType {
        Canvas,
        Button,
        Slider,
    };

    enum class ButtonState {
        Normal,
        Hover,
        Pressed,
    };

    enum class SliderState {
        Normal,
        Hover,
        Dragging,
    };
} // namespace Orbis