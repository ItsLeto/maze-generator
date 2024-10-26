#include "olcPixelGameEngine.h"

#include <bitset>
#include <cstdint>
#include <vector>

enum class Direction : uint8_t { North, East, South, West, NUM };

class Cell {
    constexpr static auto WALL_WIDTH{1};

  public:
    Cell(int32_t x, int32_t y, int32_t w, int32_t h) : m_x(x), m_y(y), m_w(w), m_h(h) {};

    void draw(olc::PixelGameEngine* pge, olc::Pixel color = olc::WHITE) {
        if (!m_walls.test(std::to_underlying(Direction::East))) {
            pge->FillRect(m_x * (m_w + WALL_WIDTH) + m_w, m_y * (m_h + WALL_WIDTH), WALL_WIDTH, m_h, color);
        }
        if (!m_walls.test(std::to_underlying(Direction::South))) {
            pge->FillRect(m_x * (m_w + WALL_WIDTH), m_y * (m_h + WALL_WIDTH) + m_h, m_w, WALL_WIDTH, color);
        }
        pge->FillRect(m_x * (m_w + WALL_WIDTH), m_y * (m_h + WALL_WIDTH), m_w, m_h, (m_visited) ? color : olc::BLUE);
    }

    void remove_wall(Direction wall) {
        m_walls.set(std::to_underlying(wall), false);
    }

    bool has_wall(Direction wall) {
        return m_walls.test(std::to_underlying(wall));
    }

    void set_visited() {
        m_visited = true;
    }

    bool is_visited() {
        return m_visited;
    }

    int32_t m_x;
    int32_t m_y;
    int32_t m_w;
    int32_t m_h;
    int32_t m_g_score{std::numeric_limits<int32_t>::max()};
    double m_f_score{std::numeric_limits<double>::max()};

    bool m_visited{false};
    std::bitset<std::to_underlying(Direction::NUM)> m_walls{std::bitset<4>().set()};
};
