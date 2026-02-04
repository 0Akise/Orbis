#pragma once

namespace Orbis {
    enum class DrawingType {
        Line,
        Rect,
        Text,
        WText,
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
        TextboxSingle,
        TextboxMulti,
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

    enum class TextboxState {
        Normal,
        Hover,
        Focused,
    };

    enum class CursorStyle {
        Line,
        Block,
        Custom,
    };
} // namespace Orbis