#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <map>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Orbis/System/Controls.hpp"
#include "Orbis/System/Drawings.hpp"

namespace Orbis {
    class Widget;
    class Canvas;
    class Button;
    class Slider;
    class TextboxSingle;
    class TextboxMulti;

    class Widget : public std::enable_shared_from_this<Widget> {
    protected:
        sf::Vector2f mSize      = {0, 0};
        sf::Vector2f mPosition  = {0, 0};
        size_t       mZLevel    = 0;
        bool         mIsVisible = true;

        std::map<std::string, std::shared_ptr<DrawingsLine>>    mDrawingsLine;
        std::map<std::string, std::shared_ptr<DrawingsRect>>    mDrawingsRect;
        std::map<std::string, std::shared_ptr<DrawingsText>>    mDrawingsText;
        std::map<std::string, std::shared_ptr<DrawingsWText>>   mDrawingsWText;
        std::map<std::string, std::shared_ptr<DrawingsTexture>> mDrawingsTexture;

    protected:
        using ColorModifier = std::function<sf::Color(DrawingType, const sf::Color&)>;

        void RenderDrawing(sf::RenderWindow& window, const std::shared_ptr<Drawings>& drawing, sf::Vector2f pos_widget, const ColorModifier& color_modifier = nullptr) {
            sf::Vector2f pos_drawing = pos_widget + drawing->mPosition;

            auto get_color = [&](const sf::Color& original) -> sf::Color {
                if (color_modifier) {
                    return color_modifier(drawing->mType, original);
                }

                return original;
            };

            switch (drawing->mType) {
                case DrawingType::Line: {
                    auto line = std::static_pointer_cast<DrawingsLine>(drawing);

                    if (line->mPoints.size() < 2) {
                        break;
                    }

                    if (line->mThickness <= 1.0f) {
                        sf::VertexArray curve(sf::PrimitiveType::LineStrip);

                        for (const auto& point : line->mPoints) {
                            curve.append(sf::Vertex(pos_drawing + point, line->mFillColor));
                        }

                        window.draw(curve);
                    }
                    else {
                        sf::VertexArray line_thick     = sf::VertexArray(sf::PrimitiveType::Triangles);
                        float           thickness_half = line->mThickness / 2.0f;

                        for (size_t i = 0; i < line->mPoints.size() - 1; ++i) {
                            sf::Vector2f p1        = pos_drawing + line->mPoints[i];
                            sf::Vector2f p2        = pos_drawing + line->mPoints[i + 1];
                            sf::Vector2f direction = p2 - p1;
                            float        length    = std::sqrt(direction.x * direction.x + direction.y * direction.y);

                            if (length < 0.001f) {
                                continue;
                            }

                            sf::Vector2f perpendicular = {-direction.y / length, direction.x / length};
                            sf::Vector2f offset        = perpendicular * thickness_half;
                            sf::Vector2f v1            = p1 + offset;
                            sf::Vector2f v2            = p1 - offset;
                            sf::Vector2f v3            = p2 - offset;
                            sf::Vector2f v4            = p2 + offset;

                            line_thick.append(sf::Vertex(v1, line->mFillColor));
                            line_thick.append(sf::Vertex(v2, line->mFillColor));
                            line_thick.append(sf::Vertex(v3, line->mFillColor));
                            line_thick.append(sf::Vertex(v1, line->mFillColor));
                            line_thick.append(sf::Vertex(v3, line->mFillColor));
                            line_thick.append(sf::Vertex(v4, line->mFillColor));
                        }

                        window.draw(line_thick);
                    }

                    break;
                }
                case DrawingType::Rect: {
                    // TODO : Implement Outlines

                    auto      rect        = std::static_pointer_cast<DrawingsRect>(drawing);
                    sf::Color state_color = get_color(rect->mFillColor);

                    if (rect->mIsRounded == true) {
                        sf::ConvexShape shape = sf::RectRounded(rect->mSize, rect->mRoundingRadius);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(state_color);

                        if (rect->mIsOutlined == true) {
                            shape.setOutlineThickness(rect->mOutlineThickness);
                            shape.setOutlineColor(rect->mOutlineColor);
                        }

                        window.draw(shape);
                    }
                    else {
                        sf::RectangleShape shape = sf::RectangleShape(rect->mSize);

                        shape.setPosition(pos_drawing);
                        shape.setFillColor(state_color);

                        if (rect->mIsOutlined == true) {
                            shape.setOutlineThickness(rect->mOutlineThickness);
                            shape.setOutlineColor(rect->mOutlineColor);
                        }

                        window.draw(shape);
                    }

                    break;
                }
                case DrawingType::Text: {
                    auto text_drawing = std::static_pointer_cast<DrawingsText>(drawing);

                    if (text_drawing->mCachedText.has_value() == false) {
                        text_drawing->mCachedText = sf::Text(*text_drawing->mFont, text_drawing->mText, text_drawing->mFontSize);
                    }

                    sf::Text&     text   = text_drawing->mCachedText.value();
                    sf::FloatRect bounds = text.getLocalBounds();
                    sf::Vector2f  offset = {0, 0};

                    float offset_x = 0.0f;
                    float offset_y = 0.0f;

                    if (text_drawing->mAlign == TextAlign::CenterTop || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -(bounds.size.x) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop || text_drawing->mAlign == TextAlign::RightCenter || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -(bounds.size.x);
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize)) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom || text_drawing->mAlign == TextAlign::CenterBottom || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize));
                    }

                    offset = {offset_x, offset_y};

                    text.setPosition(pos_drawing + offset);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }
                case DrawingType::WText: {
                    auto text_drawing = std::static_pointer_cast<DrawingsWText>(drawing);

                    if (text_drawing->mCachedText.has_value() == false) {
                        text_drawing->mCachedText = sf::Text(*text_drawing->mFont, text_drawing->mWText, text_drawing->mFontSize);
                    }

                    sf::Text&     text   = text_drawing->mCachedText.value();
                    sf::FloatRect bounds = text.getLocalBounds();
                    sf::Vector2f  offset = {0, 0};

                    float offset_x = 0.0f;
                    float offset_y = 0.0f;

                    if (text_drawing->mAlign == TextAlign::CenterTop || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::CenterBottom) {
                        offset_x = -(bounds.size.x) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::RightTop || text_drawing->mAlign == TextAlign::RightCenter || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_x = -(bounds.size.x);
                    }

                    if (text_drawing->mAlign == TextAlign::LeftCenter || text_drawing->mAlign == TextAlign::Center || text_drawing->mAlign == TextAlign::RightCenter) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize)) / 2.0f;
                    }
                    else if (text_drawing->mAlign == TextAlign::LeftBottom || text_drawing->mAlign == TextAlign::CenterBottom || text_drawing->mAlign == TextAlign::RightBottom) {
                        offset_y = -(static_cast<float>(text_drawing->mFontSize));
                    }

                    offset = {offset_x, offset_y};

                    text.setPosition(pos_drawing + offset);
                    text.setFillColor(text_drawing->mFillColor);
                    window.draw(text);

                    break;
                }
                case DrawingType::Texture: {
                    auto      texture     = std::static_pointer_cast<DrawingsTexture>(drawing);
                    sf::Color final_color = get_color(texture->mFillColor);

                    if (texture->mTextureSmoothing == true && texture->mTexture->isSmooth() == false) {
                        texture->mTexture->setSmooth(true);
                    }

                    sf::RectangleShape shape(texture->mSize);

                    shape.setPosition(pos_drawing);
                    shape.setTexture(texture->mTexture.get());
                    shape.setFillColor(final_color);
                    window.draw(shape);

                    break;
                }
            }
        }

        void RenderAllDrawings(sf::RenderWindow& window, sf::Vector2f pos_widget, const ColorModifier& color_modifier = nullptr) {
            std::vector<std::pair<size_t, std::shared_ptr<Drawings>>> all_drawings;

            for (const auto& [id, drawing] : mDrawingsLine) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsRect) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsWText) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                all_drawings.push_back({drawing->mZLevel, drawing});
            }

            std::sort(all_drawings.begin(), all_drawings.end(), [](const auto& a, const auto& b) {
                return a.first < b.first;
            });

            for (const auto& [zlevel, drawing] : all_drawings) {
                RenderDrawing(window, drawing, pos_widget, color_modifier);
            }
        }

        void CloneDrawingsTo(Widget* target) const {
            for (const auto& [id, drawing] : mDrawingsLine) {
                auto cloned_drawing = std::make_shared<DrawingsLine>(*drawing);

                target->mDrawingsLine[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsRect) {
                auto cloned_drawing = std::make_shared<DrawingsRect>(*drawing);

                target->mDrawingsRect[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsText) {
                auto cloned_drawing = std::make_shared<DrawingsText>(*drawing);

                target->mDrawingsText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsWText) {
                auto cloned_drawing = std::make_shared<DrawingsWText>(*drawing);

                target->mDrawingsWText[id] = cloned_drawing;
            }

            for (const auto& [id, drawing] : mDrawingsTexture) {
                auto cloned_drawing = std::make_shared<DrawingsTexture>(*drawing);

                target->mDrawingsTexture[id] = cloned_drawing;
            }
        }

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

        DrawingsWText& GetWText(const std::string& id) {
            auto iter = mDrawingsWText.find(id);

            if (iter == mDrawingsWText.end()) {
                throw std::runtime_error("DrawingsWText with id '" + id + "' not found");
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

        Widget& DrawLine(const std::string& id, const std::vector<sf::Vector2f>& points, size_t zlevel, sf::Color color, float thickness = 2.0f) {
            auto drawing = std::make_shared<DrawingsLine>();

            drawing->mType      = DrawingType::Line;
            drawing->mID        = id;
            drawing->mPoints    = points;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = color;
            drawing->mThickness = thickness;

            mDrawingsLine[id] = drawing;

            return *this;
        }

        Widget& DrawRect(const std::string& id, sf::Vector2f size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, bool is_outlined = false, float outline_thickness = 0.0f, sf::Color outline_color = sf::Color::Black, bool is_rounded = false, float rounding_radius = 0.0f) {
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

        Widget& DrawText(const std::string& id, size_t font_size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Font> font, TextAlign align = TextAlign::LeftTop, const std::string& text = "") {
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

        Widget& DrawWText(const std::string& id, size_t font_size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Font> font, TextAlign align = TextAlign::LeftTop, const std::wstring& wtext = L"") {
            auto drawing = std::make_shared<DrawingsWText>();

            drawing->mType      = DrawingType::WText;
            drawing->mID        = id;
            drawing->mFontSize  = font_size;
            drawing->mPosition  = position;
            drawing->mZLevel    = zlevel;
            drawing->mFillColor = fill_color;
            drawing->mFont      = font;
            drawing->mAlign     = align;
            drawing->mWText     = wtext;

            mDrawingsWText[id] = drawing;

            return *this;
        }

        Widget& DrawTexture(const std::string& id, sf::Vector2f size, sf::Vector2f position, size_t zlevel, sf::Color fill_color, std::shared_ptr<sf::Texture> texture, bool smoothing_enabled = false) {
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
} // namespace Orbis