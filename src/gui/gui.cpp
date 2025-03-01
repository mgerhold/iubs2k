#include <fmt/format.h>
#include <gui/gui.hpp>
#include <ranges>

[[nodiscard]] Gui::Gui()
    : m_window{ sf::VideoMode{ { 1024, 768 } },
                "Inherently Unsafe Backseat System 2k",
                sf::Style::Titlebar | sf::Style::Close } {
    static constexpr auto font_path = "resources/fonts/JetBrainsMono-Regular.ttf";
    if (not m_font.openFromFile(font_path)) {
        throw std::runtime_error{ fmt::format("Unable to load font {}.", font_path) };
    }
}

void Gui::update(Emulator const& emulator) {
    if (not m_window.isOpen()) {
        m_is_running = false;
        return;
    }

    while (auto const event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
    }

    m_window.clear(sf::Color::Black);

    auto const& text_device = emulator.text_device();

    auto const text_contents = text_device.text()
                               | std::views::transform([](char const c) { return c == '\0' ? ' ' : c; })
                               | std::ranges::to<std::string>();

    auto const text = sf::Text{ m_font, text_contents, 20 };

    m_window.draw(text);

    m_window.display();
}
