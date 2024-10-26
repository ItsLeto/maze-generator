#include "olcPixelGameEngine.h"

#include <bitset>
#include <cstdint>
#include <vector>

enum class Direction : uint8_t { North, East, South, West, NUM };

class Cell {
    constexpr static auto WALL_WIDTH{1};

  public:
    Cell(std::size_t x, std::size_t y, std::size_t w, std::size_t h) : m_x(x), m_y(y), m_w(w), m_h(h) {};

    void draw(olc::PixelGameEngine* pge, olc::Pixel color = olc::WHITE) {
        if (!m_walls.test(std::to_underlying(Direction::East))) {
            pge->FillRect(m_x * (m_w + WALL_WIDTH) + m_w, m_y * (m_h + WALL_WIDTH), WALL_WIDTH, m_h);
        }
        if (!m_walls.test(std::to_underlying(Direction::South))) {
            pge->FillRect(m_x * (m_w + WALL_WIDTH), m_y * (m_h + WALL_WIDTH) + m_h, m_w, WALL_WIDTH);
        }
        pge->FillRect(m_x * (m_w + WALL_WIDTH), m_y * (m_h + WALL_WIDTH), m_w, m_h, (m_visited) ? color : olc::BLUE);
    }

    void remove_wall(Direction wall) {
        m_walls.set(std::to_underlying(wall), false);
    }

    void set_visited() {
        m_visited = true;
    }

    bool is_visited() {
        return m_visited;
    }

    std::size_t m_x;
    std::size_t m_y;
    std::size_t m_w;
    std::size_t m_h;

    bool m_visited{false};
    std::bitset<std::to_underlying(Direction::NUM)> m_walls{std::bitset<4>().set()};
};
