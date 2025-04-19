#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <SFML/Graphics.hpp>

namespace Orbis {
    class ResourceVault {
    private:
        std::unordered_map<std::string, std::shared_ptr<sf::Font>> mFonts;
        std::unordered_map<std::string, std::shared_ptr<sf::Texture>> mTextures;
        std::unordered_map<std::string, std::shared_ptr<sf::Sprite>> mSprites;

    public:
        ResourceVault() = default;

        std::shared_ptr<sf::Font> LoadFont(const std::string& path) {
            std::string key = path;
            auto iter = mFonts.find(key);

            if (iter != mFonts.end()) {
                return iter->second;
            }

            auto font = std::make_shared<sf::Font>();

            if (font->openFromFile(key) == false) {
                throw std::runtime_error("Failed to load font: " + path);
            }

            mFonts[key] = font;

            return font;
        }

        std::shared_ptr<sf::Texture> LoadTexture(
            const std::string& path,
            bool srgb_enabled = false,
            const sf::IntRect& area = sf::IntRect()) {
            std::string key = path;

            if (area != sf::IntRect()) {
                key += "_" + std::to_string(area.position.x) +
                       "_" + std::to_string(area.position.y) +
                       "_" + std::to_string(area.size.x) +
                       "_" + std::to_string(area.size.y);
            }

            if (srgb_enabled == true) {
                key += "_srgb";
            }

            auto iter = mTextures.find(key);

            if (iter != mTextures.end()) {
                return iter->second;
            }

            auto texture = std::make_shared<sf::Texture>();

            if (texture->loadFromFile(path, srgb_enabled, area) == false) {
                throw std::runtime_error("Failed to load texture: " + path);
            }

            mTextures[key] = texture;

            return texture;
        }

        void ClearFonts() {
            mFonts.clear();
        }

        void ClearTextures() {
            mTextures.clear();
        }

        void ClearSprites() {
            mSprites.clear();
        }

        void ClearAllResources() {
            ClearFonts();
            ClearTextures();
            ClearSprites();
        }
    };
}