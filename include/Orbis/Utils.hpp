#pragma once

#include <chrono>
#include <cmath>

namespace Orbis {
    class Utils {
    public:
        static float GetElapsedSeconds(
            const std::chrono::steady_clock::time_point& start,
            const std::chrono::steady_clock::time_point& end) {
            return std::chrono::duration<float>(end - start).count();
        }

        static float GetTimeFactor(
            const std::chrono::steady_clock::time_point& start,
            float durationSeconds) {
            auto now = std::chrono::steady_clock::now();
            float elapsed = GetElapsedSeconds(start, now);
            return std::min(1.0f, elapsed / durationSeconds);
        }

        static float SmoothLerp(
            float from,
            float to,
            std::chrono::steady_clock::time_point& start,
            float duration_in_seconds) {
            float t = GetTimeFactor(start, duration_in_seconds);

            t = std::max(0.0f, std::min(1.0f, t));

            float smooth = t * t * (3.0f - 2.0f * t);

            return from + smooth * (to - from);
        }
    };
}