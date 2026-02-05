#pragma once

#include "Orbis/SFML/Shapes.hpp"
#include "Orbis/Widgets/Widget.hpp"

namespace Orbis {
    class TextboxSingle : public Widget {
    private:
        std::function<void(const sf::String&)> mOnTextChanged;
        std::function<void()>                  mOnEnterPressed;

        sf::String  mText        = "";
        sf::String  mPlaceholder = "";
        std::string mEditableID  = "";

        size_t mCursorPos      = 0;
        size_t mSelectionStart = 0;
        size_t mSelectionEnd   = 0;

        TextboxState mState = TextboxState::Normal;
        // CursorStyle  mCursorStyle = CursorStyle::Line;

        bool                                  mIsCursorVisible = false;
        std::chrono::steady_clock::time_point mCursorLastBlink;
        float                                 mCursorBlinkInterval = 0.5f;

        float mScrollOffset = 0.0f;
        float mPadding      = 5.0f;

        float GetCursorPosX() const {
            if (mEditableID.empty() == true) {
                return 0.0f;
            }

            const DrawingsText& text_drawing = GetText(mEditableID);

            if (mText.isEmpty() == true || mCursorPos == 0) {
                return 0.0f;
            }

            sf::Text temp = sf::Text(*text_drawing.mFont, mText.substring(0, mCursorPos), text_drawing.mFontSize);

            return temp.getLocalBounds().size.x;
        }

        size_t GetCursorPosFromMouseX(float mouse_x, sf::Vector2f widget_pos) const {
            if (mEditableID.empty() == true) {
                return 0;
            }

            const DrawingsText& text_drawing = GetText(mEditableID);
            float               adjusted_x   = mouse_x - widget_pos.x - mPadding + mScrollOffset;

            if (adjusted_x <= 0.0f) {
                return 0;
            }

            for (size_t i = 0; i <= mText.getSize(); ++i) {
                sf::Text temp = sf::Text(*text_drawing.mFont, mText.substring(0, i), text_drawing.mFontSize);
                float    x    = temp.getLocalBounds().size.x;

                if (adjusted_x <= x) {
                    if (i > 0) {
                        sf::Text prev   = sf::Text(*text_drawing.mFont, mText.substring(0, i - 1), text_drawing.mFontSize);
                        float    prev_x = prev.getLocalBounds().size.x;

                        if (adjusted_x - prev_x < x - adjusted_x) {
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
            if (mEditableID.empty() == true) {
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

        TextboxSingle& SetEdiableText(const std::string& text_id) {
            mEditableID = text_id;

            return *this;
        }

        TextboxSingle& SetCursorBlinkInterval(float seconds) {
            mCursorBlinkInterval = seconds;

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
            cloned->mEditableID     = mEditableID;
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
        }
    };
} // namespace Orbis