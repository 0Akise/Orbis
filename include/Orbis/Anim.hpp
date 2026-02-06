#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <optional>

#include <SFML/Graphics.hpp>

namespace Orbis {
    class Anim {
    public:
        static float GetElapsedSeconds(const std::chrono::steady_clock::time_point& start, const std::chrono::steady_clock::time_point& end) {
            return std::chrono::duration<float>(end - start).count();
        }

        static float GetTimeFactor(const std::chrono::steady_clock::time_point& start, float durationSeconds) {
            auto  now     = std::chrono::steady_clock::now();
            float elapsed = GetElapsedSeconds(start, now);

            return std::min(1.0f, elapsed / durationSeconds);
        }

        static float SmoothLerp(float from, float to, std::chrono::steady_clock::time_point& start, float duration_in_seconds) {
            float t = GetTimeFactor(start, duration_in_seconds);

            t = std::max(0.0f, std::min(1.0f, t));

            float smooth = t * t * (3.0f - 2.0f * t);

            return from + smooth * (to - from);
        }

        static float EaseInOutCubic(float t) {
            return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
        }

        static float EaseOutQuad(float t) {
            return 1.0f - (1.0f - t) * (1.0f - t);
        }

        static float EaseInQuad(float t) {
            return t * t;
        }

        static float EaseOutBounce(float t) {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;

            if (t < 1.0f / d1) {
                return n1 * t * t;
            }
            else if (t < 2.0f / d1) {
                t -= 1.5f / d1;
                return n1 * t * t + 0.75f;
            }
            else if (t < 2.5f / d1) {
                t -= 2.25f / d1;
                return n1 * t * t + 0.9375f;
            }
            else {
                t -= 2.625f / d1;
                return n1 * t * t + 0.984375f;
            }
        }

        static float EaseInBounce(float t) {
            return 1.0f - EaseOutBounce(1.0f - t);
        }

        static float EaseOutElastic(float t) {
            const float c4 = (2.0f * 3.14159f) / 3.0f;

            if (t == 0.0f) {
                return 0.0f;
            }
            if (t == 1.0f) {
                return 1.0f;
            }

            return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
        }

        static sf::Vector2f LerpVector(sf::Vector2f from, sf::Vector2f to, float t) {
            return {from.x + (to.x - from.x) * t, from.y + (to.y - from.y) * t};
        }

        static sf::Vector2f SmoothLerpVector(sf::Vector2f from, sf::Vector2f to, std::chrono::steady_clock::time_point& start, float duration) {
            float t = GetTimeFactor(start, duration);

            t = EaseOutQuad(std::max(0.0f, std::min(1.0f, t)));

            return LerpVector(from, to, t);
        }

        static std::vector<sf::Vector2f> GeneratePoints(std::function<float(float)> func, float x_min, float x_max, float step, float scale_x, float scale_y, sf::Vector2f origin) {
            std::vector<sf::Vector2f> points;

            for (float x = x_min; x <= x_max; x += step) {
                float y        = func(x);
                float screen_x = origin.x + (x * scale_x);
                float screen_y = origin.y - (y * scale_y);

                points.push_back({screen_x, screen_y});
            }

            return points;
        }
    };

    struct AnimationState {
        bool                                  mIsActive = false;
        std::chrono::steady_clock::time_point mStartTime;
        float                                 mDuration = 0.0f;

        sf::Vector2f mStartPos  = {0.0f, 0.0f};
        sf::Vector2f mTargetPos = {0.0f, 0.0f};

        std::function<void()>       mOnComplete = nullptr;
        std::function<float(float)> mEasingFunc = Anim::EaseOutQuad;

        void SetEasing(std::function<float(float)> easing_func) {
            mEasingFunc = easing_func;
        }

        bool IsComplete() const {
            if (mIsActive == false) {
                return false;
            }

            auto now = std::chrono::steady_clock::now();

            return mDuration <= Anim::GetElapsedSeconds(mStartTime, now);
        }

        sf::Vector2f GetCurrentPosition() const {
            if (mIsActive == false) {
                return mStartPos;
            }

            float t = Anim::GetTimeFactor(mStartTime, mDuration);

            t = std::max(0.0f, std::min(1.0f, t));
            t = mEasingFunc(t);

            return Anim::LerpVector(mStartPos, mTargetPos, t);
        }

        void Start(sf::Vector2f start, sf::Vector2f target, float duration, std::function<void()> on_complete = nullptr) {
            mIsActive   = true;
            mStartTime  = std::chrono::steady_clock::now();
            mDuration   = duration;
            mStartPos   = start;
            mTargetPos  = target;
            mOnComplete = std::move(on_complete);
        }
    };

    class DelayedCallback {
    private:
        bool                                  mIsActive = false;
        std::chrono::steady_clock::time_point mStartTime;
        float                                 mDelay    = 0.0f;
        std::function<void()>                 mCallback = nullptr;

    public:
        bool IsActive() const {
            return mIsActive;
        }

        void Start(float delay_seconds, std::function<void()> callback) {
            mIsActive  = true;
            mStartTime = std::chrono::steady_clock::now();
            mDelay     = delay_seconds;
            mCallback  = std::move(callback);
        }

        void Update() {
            if (mIsActive == false) {
                return;
            }

            auto  now     = std::chrono::steady_clock::now();
            float elapsed = Anim::GetElapsedSeconds(mStartTime, now);

            if (elapsed >= mDelay) {
                if (mCallback) {
                    mCallback();
                }

                mIsActive = false;
            }
        }

        void Cancel() {
            mIsActive = false;
        }
    };
} // namespace Orbis