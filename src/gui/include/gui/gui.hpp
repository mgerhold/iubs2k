#pragma once

#include <SFML/Graphics.hpp>
#include <emulator/emulator.hpp>

class Gui final {
private:
    sf::RenderWindow m_window;
    sf::Font m_font;
    bool m_is_running = true;

public:
    [[nodiscard]] Gui();

    void update(Emulator const& emulator);

    [[nodiscard]] bool is_running() const {
        return m_is_running;
    }
};
