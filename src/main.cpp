#include "example.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode({1080, 720}), "Orbis Examples", sf::Style::Default);
    OrbisExample::Program program = OrbisExample::Program::Create();

    window.setFramerateLimit(120);
    window.setVerticalSyncEnabled(false);

    program.Run(window);

    return 0;
}