#pragma once

#include <cfloat>
#include <limits>

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/Widgets/Widget.hpp"

namespace Orbis {
    class TextboxSingle : public Widget {
    private:
        std::function<void(const sf::String&)> mOnTextChanged;
        std::function<void()>                  mOnEnterPressed;

        sf::String  mText        = "";
        sf::String  mPlaceholder = "";
        std::string mIDEditable  = "";

        size_t mCursorPos      = 0;
        size_t mSelectionStart = 0;
        size_t mSelectionEnd   = 0;

        TextboxState mState = TextboxState::Normal;
        // CursorStyle  mCursorStyle = CursorStyle::Line;
        bool mIsWideText = false;

        bool                                  mIsCursorVisible = false;
        std::chrono::steady_clock::time_point mCursorLastBlink;
        float                                 mCursorBlinkInterval = 0.5f;

        float mScrollOffset = 0.0f;
        float mPadding      = 5.0f;

        float GetCursorPosX() const {
            if (mIDEditable.empty() == true) {
                return 0.0f;
            }

            if (mText.isEmpty() == true || mCursorPos == 0) {
                return 0.0f;
            }

            if (mIsWideText == false) {
                const DrawingsText& text_drawing = GetText(mIDEditable);
                sf::Text            temp         = sf::Text(*text_drawing.mFont, mText.substring(0, mCursorPos), text_drawing.mFontSize);

                return temp.getLocalBounds().size.x;
            }
            else {
                const DrawingsWText& text_drawing = GetWText(mIDEditable);
                sf::Text             temp         = sf::Text(*text_drawing.mFont, mText.substring(0, mCursorPos), text_drawing.mFontSize);

                return temp.getLocalBounds().size.x;
            }
        }

        size_t GetCursorPosFromMouseX(float mouse_x, sf::Vector2f widget_pos) const {
            if (mIDEditable.empty() == true) {
                return 0;
            }

            float adjusted_x = mouse_x - widget_pos.x - mPadding + mScrollOffset;

            if (adjusted_x <= 0.0f) {
                return 0;
            }

            std::shared_ptr<sf::Font> font;
            size_t                    font_size;

            if (mIsWideText == false) {
                const DrawingsText& text_drawing = GetText(mIDEditable);
                font                             = text_drawing.mFont;
                font_size                        = text_drawing.mFontSize;
            }
            else {
                const DrawingsWText& text_drawing = GetWText(mIDEditable);
                font                              = text_drawing.mFont;
                font_size                         = text_drawing.mFontSize;
            }

            for (size_t i = 0; i <= mText.getSize(); ++i) {
                sf::Text temp = sf::Text(*font, mText.substring(0, i), font_size);
                float    x    = temp.getLocalBounds().size.x;

                if (adjusted_x <= x) {
                    if (0 < i) {
                        sf::Text prev   = sf::Text(*font, mText.substring(0, i - 1), font_size);
                        float    prev_x = prev.getLocalBounds().size.x;

                        if ((adjusted_x - prev_x) < (x - adjusted_x)) {
                            return i - 1;
                        }
                    }

                    return i;
                }
            }

            return mText.getSize();
        }

        void MoveCursor(int delta, bool selecting) {
            size_t new_pos = mCursorPos;

            if (delta < 0) {
                if (0 < new_pos) {
                    new_pos--;
                }
            }
            else if (0 < delta) {
                if (new_pos < mText.getSize()) {
                    new_pos++;
                }
            }

            if (selecting == true) {
                if (mSelectionStart == mSelectionEnd) {
                    mSelectionStart = mCursorPos;
                }

                mSelectionEnd = new_pos;
            }
            else {
                mSelectionStart = mSelectionEnd = 0;
            }

            mCursorPos = new_pos;
        }

        void DeleteSelection() {
            if (mSelectionStart == mSelectionEnd) {
                return;
            }

            size_t start = std::min(mSelectionStart, mSelectionEnd);
            size_t end   = std::max(mSelectionStart, mSelectionEnd);

            sf::String before = mText.substring(0, start);
            sf::String after  = mText.substring(end);

            mText           = before + after;
            mCursorPos      = start;
            mSelectionStart = mSelectionEnd = 0;

            if (mOnTextChanged) {
                mOnTextChanged(mText);
            }
        }

        void InsertText(const sf::String& text) {
            if (mSelectionStart != mSelectionEnd) {
                DeleteSelection();
            }

            sf::String before = mText.substring(0, mCursorPos);
            sf::String after  = mText.substring(mCursorPos);

            mText = before + text + after;
            mCursorPos += text.getSize();

            if (mOnTextChanged) {
                mOnTextChanged(mText);
            }
        }

        void DeleteAtCursor(bool forward = false) {
            if (mSelectionStart != mSelectionEnd) {
                DeleteSelection();

                return;
            }

            if (forward == true) {
                if (mCursorPos < mText.getSize()) {
                    sf::String before = mText.substring(0, mCursorPos);
                    sf::String after  = mText.substring(mCursorPos + 1);

                    mText = before + after;

                    if (mOnTextChanged) {
                        mOnTextChanged(mText);
                    }
                }
            }
            else {
                if (0 < mCursorPos) {
                    sf::String before = mText.substring(0, mCursorPos - 1);
                    sf::String after  = mText.substring(mCursorPos);

                    mText = before + after;
                    mCursorPos--;

                    if (mOnTextChanged) {
                        mOnTextChanged(mText);
                    }
                }
            }
        }

        void UpdateScrollOffset() {
            if (mIDEditable.empty() == true) {
                return;
            }

            float cursor_x      = GetCursorPosX();
            float visible_width = mSize.x - 2 * mPadding;

            if (visible_width < cursor_x - mScrollOffset) {
                mScrollOffset = cursor_x - visible_width;
            }

            if (cursor_x < mScrollOffset) {
                mScrollOffset = cursor_x;
            }

            if (mScrollOffset < 0.0f) {
                mScrollOffset = 0.0f;
            }
        }

    public:
        TextboxSingle() = default;

        DrawingsText& GetText(const std::string& id) const {
            auto iter = mDrawingsText.find(id);

            if (iter == mDrawingsText.end()) {
                throw std::runtime_error("DrawingsText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        DrawingsWText& GetWText(const std::string& id) const {
            auto iter = mDrawingsWText.find(id);

            if (iter == mDrawingsWText.end()) {
                throw std::runtime_error("DrawingsWText with id '" + id + "' not found");
            }

            return *iter->second;
        }

        TextboxSingle& SetOnTextChanged(std::function<void(const sf::String&)> callback) {
            mOnTextChanged = std::move(callback);

            return *this;
        }

        TextboxSingle& SetOnEnterPressed(std::function<void()> callback) {
            mOnEnterPressed = std::move(callback);

            return *this;
        }

        TextboxSingle& SetPlaceholder(const sf::String& placeholder) {
            mPlaceholder = placeholder;

            return *this;
        }

        TextboxSingle& SetEditableText(const std::string& text_id) {
            mIDEditable = text_id;
            mIsWideText = false;

            return *this;
        }

        TextboxSingle& SetEditableWText(const std::string& wtext_id) {
            mIDEditable = wtext_id;
            mIsWideText = true;

            return *this;
        }

        TextboxSingle& SetCursorBlinkInterval(float seconds) {
            mCursorBlinkInterval = seconds;

            return *this;
        }

        TextboxSingle& BindInt(int* value_ptr, int min_value = INT_MIN, int max_value = INT_MAX) {
            if (value_ptr == nullptr) {
                throw std::runtime_error("BindInt: value_ptr cannot be null");
            }

            mOnTextChanged = [this, value_ptr, min_value, max_value](const sf::String& text) {
                std::string text_str = text.toAnsiString();

                if (text_str.empty() == true) {
                    return;
                }

                try {
                    size_t pos    = 0;
                    int    parsed = std::stoi(text_str, &pos);

                    if (pos != text_str.length()) {
                        return;
                    }

                    int clamped = std::max(min_value, std::min(max_value, parsed));

                    *value_ptr = clamped;

                    if (clamped != parsed) {
                        mText      = std::to_string(clamped);
                        mCursorPos = mText.getSize();

                        UpdateScrollOffset();
                    }
                } catch (const std::invalid_argument&) {
                    return;
                } catch (const std::out_of_range&) {
                    if (text_str[0] == '-') {
                        *value_ptr = min_value;
                        mText      = std::to_string(min_value);
                    }
                    else {
                        *value_ptr = max_value;
                        mText      = std::to_string(max_value);
                    }

                    mCursorPos = mText.getSize();

                    UpdateScrollOffset();
                }
            };

            return *this;
        }

        TextboxSingle& BindFloat(float* value_ptr, float min_value = -FLT_MAX, float max_value = FLT_MAX) {
            if (value_ptr == nullptr) {
                throw std::runtime_error("BindFloat: value_ptr cannot be null");
            }

            mOnTextChanged = [this, value_ptr, min_value, max_value](const sf::String& text) {
                std::string text_str = text.toAnsiString();

                if (text_str.empty() == true) {
                    return;
                }

                try {
                    size_t pos    = 0;
                    float  parsed = std::stof(text_str, &pos);

                    if (pos != text_str.length()) {
                        return;
                    }

                    float clamped = std::max(min_value, std::min(max_value, parsed));

                    *value_ptr = clamped;

                    if (std::abs(clamped - parsed) > 0.0001f) {
                        mText      = std::to_string(clamped);
                        mCursorPos = mText.getSize();

                        UpdateScrollOffset();
                    }

                } catch (const std::invalid_argument&) {
                    return;
                } catch (const std::out_of_range&) {
                    if (text_str[0] == '-') {
                        *value_ptr = min_value;
                        mText      = std::to_string(min_value);
                    }
                    else {
                        *value_ptr = max_value;
                        mText      = std::to_string(max_value);
                    }

                    mCursorPos = mText.getSize();

                    UpdateScrollOffset();
                }
            };

            return *this;
        }

        std::shared_ptr<Widget> CloneImpl() const override {
            auto cloned = std::make_shared<TextboxSingle>();

            cloned->mSize           = mSize;
            cloned->mPosition       = mPosition;
            cloned->mZLevel         = mZLevel;
            cloned->mIsVisible      = mIsVisible;
            cloned->mText           = mText;
            cloned->mPlaceholder    = mPlaceholder;
            cloned->mIDEditable     = mIDEditable;
            cloned->mOnTextChanged  = mOnTextChanged;
            cloned->mOnEnterPressed = mOnEnterPressed;

            CloneDrawingsTo(cloned.get());

            return cloned;
        }

        void UpdateImpl(const Controls& controls, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f  pos_global = pos_panel + mPosition;
            sf::FloatRect bounds(pos_global, mSize);

            bool is_hovered   = bounds.contains(controls.mMouse.mPosition);
            auto left_pressed = std::find(controls.mMouse.mButtonsPressed.begin(), controls.mMouse.mButtonsPressed.end(), sf::Mouse::Button::Left);

            if (left_pressed != controls.mMouse.mButtonsPressed.end()) {
                if (is_hovered == true) {
                    mState          = TextboxState::Focused;
                    mCursorPos      = GetCursorPosFromMouseX(controls.mMouse.mPosition.x, pos_global);
                    mSelectionStart = mSelectionEnd = 0;
                    mIsCursorVisible                = true;
                    mCursorLastBlink                = std::chrono::steady_clock::now();
                }
                else {
                    mState          = TextboxState::Normal;
                    mSelectionStart = mSelectionEnd = 0;
                }
            }
            else {
                if (mState != TextboxState::Focused && is_hovered == true) {
                    mState = TextboxState::Hover;
                }
                else if (mState != TextboxState::Focused) {
                    mState = TextboxState::Normal;
                }
            }

            if (mState == TextboxState::Focused) {
                if (controls.mKeyboard.HasText() == true) {
                    InsertText(controls.mKeyboard.mTextEntered);
                    UpdateScrollOffset();

                    mIsCursorVisible = true;
                    mCursorLastBlink = std::chrono::steady_clock::now();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Left) == true) {
                    MoveCursor(-1, controls.mKeyboard.mIsSPressed);
                    UpdateScrollOffset();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Right) == true) {
                    MoveCursor(1, controls.mKeyboard.mIsSPressed);
                    UpdateScrollOffset();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Home) == true) {
                    if (controls.mKeyboard.mIsSPressed == true) {
                        if (mSelectionStart == mSelectionEnd) {
                            mSelectionStart = mCursorPos;
                        }

                        mSelectionEnd = 0;
                    }
                    else {
                        mSelectionStart = mSelectionEnd = 0;
                    }

                    mCursorPos = 0;

                    UpdateScrollOffset();
                }
                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::End) == true) {
                    if (controls.mKeyboard.mIsSPressed == true) {
                        if (mSelectionStart == mSelectionEnd) {
                            mSelectionStart = mCursorPos;
                        }

                        mSelectionEnd = mText.getSize();
                    }
                    else {
                        mSelectionStart = mSelectionEnd = 0;
                    }

                    mCursorPos = mText.getSize();

                    UpdateScrollOffset();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Backspace) == true) {
                    DeleteAtCursor(false);
                    UpdateScrollOffset();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Delete) == true) {
                    DeleteAtCursor(true);
                    UpdateScrollOffset();
                }

                if (controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::Enter) == true) {
                    if (mOnEnterPressed) {
                        mOnEnterPressed();
                    }
                }

                if (controls.mKeyboard.mIsCPressed == true &&
                    controls.mKeyboard.IsKeyPressed(sf::Keyboard::Key::A) == true) {
                    mSelectionStart = 0;
                    mSelectionEnd   = mText.getSize();
                }

                // TODO: Ctrl+C, Ctrl+V, Ctrl+X (clipboard operations)

                auto  now     = std::chrono::steady_clock::now();
                float elapsed = std::chrono::duration<float>(now - mCursorLastBlink).count();

                if (elapsed >= mCursorBlinkInterval) {
                    mIsCursorVisible = !mIsCursorVisible;
                    mCursorLastBlink = now;
                }
            }
        }

        void RenderImpl(sf::RenderWindow& window, sf::Vector2f pos_panel) override {
            if (mIsVisible == false) {
                return;
            }

            sf::Vector2f pos_global = pos_panel + mPosition;

            RenderAllDrawings(window, pos_global);

            if (mIDEditable.empty() == true) {
                return;
            }

            std::shared_ptr<sf::Font> font;
            size_t                    font_size;
            sf::Color                 fill_color;
            sf::Vector2f              drawing_position;

            if (mIsWideText == false) {
                const DrawingsText& text_drawing = GetText(mIDEditable);

                font             = text_drawing.mFont;
                font_size        = text_drawing.mFontSize;
                fill_color       = text_drawing.mFillColor;
                drawing_position = text_drawing.mPosition;
            }
            else {
                const DrawingsWText& text_drawing = GetWText(mIDEditable);

                font             = text_drawing.mFont;
                font_size        = text_drawing.mFontSize;
                fill_color       = text_drawing.mFillColor;
                drawing_position = text_drawing.mPosition;
            }

            sf::Vector2f text_pos = pos_global + drawing_position;

            text_pos.x += mPadding - mScrollOffset;

            if (mText.isEmpty() == true && mState != TextboxState::Focused) {
                if (mPlaceholder.isEmpty() == false) {
                    sf::Text placeholder_text(*font, mPlaceholder, font_size);

                    placeholder_text.setPosition(text_pos);
                    placeholder_text.setFillColor(sf::Color(150, 150, 150, 255));
                    window.draw(placeholder_text);
                }

                return;
            }

            if (mSelectionStart != mSelectionEnd && mState == TextboxState::Focused) {
                size_t             selection_start  = std::min(mSelectionStart, mSelectionEnd);
                size_t             selection_end    = std::max(mSelectionStart, mSelectionEnd);
                sf::Text           selection_before = sf::Text(*font, mText.substring(0, selection_start), font_size);
                sf::Text           selected         = sf::Text(*font, mText.substring(selection_start, selection_end - selection_start), font_size);
                float              before_width     = selection_before.getLocalBounds().size.x;
                float              sel_width        = selected.getLocalBounds().size.x;
                sf::RectangleShape highlight        = sf::RectangleShape({sel_width, static_cast<float>(font_size)});

                highlight.setPosition({text_pos.x + before_width, text_pos.y});
                highlight.setFillColor(sf::Color(100, 150, 255, 128));
                window.draw(highlight);
            }

            sf::Text display_text = sf::Text(*font, mText, font_size);

            display_text.setPosition(text_pos);
            display_text.setFillColor(fill_color);
            window.draw(display_text);

            if (mState == TextboxState::Focused && mIsCursorVisible == true) {
                float              cursor_x = text_pos.x + GetCursorPosX();
                sf::RectangleShape cursor   = sf::RectangleShape({2.0f, static_cast<float>(font_size)});

                cursor.setPosition({cursor_x, text_pos.y});
                cursor.setFillColor(fill_color);
                window.draw(cursor);
            }
        }
    };
} // namespace Orbis