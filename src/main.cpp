#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <array>
#include <bitset>
#include <cstdint>
#include <ranges>
#include <stack>

constexpr static auto COLS{30};
constexpr static auto ROWS{20};
constexpr static auto CELL_WIDTH{10};
constexpr static auto WALL_WIDTH{1};
constexpr static auto WINDOW_WIDTH{COLS * (CELL_WIDTH + WALL_WIDTH)};
constexpr static auto WINDOW_HIGHT{ROWS * (CELL_WIDTH + WALL_WIDTH)};

enum class Direction { North, East, South, West, NUM };
enum class Wall { North, East, South, West, NUM };

class Cell {
  public:
    Cell(std::size_t x, std::size_t y) : m_pos{x, y} {};

    std::vector<Direction> get_neighbours() {
        std::vector<Direction> neighbours;
        if (m_pos.y > 0) {
            neighbours.emplace_back(Direction::North);
        }
        if (m_pos.x < COLS - 1) {
            neighbours.emplace_back(Direction::East);
        }
        if (m_pos.y < ROWS - 1) {
            neighbours.emplace_back(Direction::South);
        }
        if (m_pos.x > 0) {
            neighbours.emplace_back(Direction::West);
        }
        return neighbours;
    }

    void draw(olc::PixelGameEngine* pge, olc::Pixel color = olc::WHITE) {

        if (!m_walls.test(std::to_underlying(Wall::East))) {
            pge->FillRect(m_pos.x * (m_width + WALL_WIDTH) + m_width,
                          m_pos.y * (m_width + WALL_WIDTH),
                          WALL_WIDTH,
                          m_width);
        }
        if (!m_walls.test(std::to_underlying(Wall::South))) {
            pge->FillRect(m_pos.x * (m_width + WALL_WIDTH),
                          m_pos.y * (m_width + WALL_WIDTH) + m_width,
                          m_width,
                          WALL_WIDTH);
        }

        pge->FillRect(m_pos.x * (m_width + WALL_WIDTH),
                      m_pos.y * (m_width + WALL_WIDTH),
                      m_width,
                      m_width,
                      (m_visited) ? color : olc::BLUE);
    }

    void remove_wall(Wall wall) {
        m_walls.set(std::to_underlying(wall), false);
    }

    olc::v_2d<std::size_t> m_pos;
    std::size_t m_width{CELL_WIDTH};
    bool m_visited{false};
    std::bitset<std::to_underlying(Wall::NUM)> m_walls{std::bitset<4>().set()};
};

class MazeGenerator : public olc::PixelGameEngine {

  public:
    MazeGenerator() {
        sAppName.assign("MazeGenerator");
    }

    bool OnUserCreate() override {
        srand(static_cast<unsigned>(time(0)));

        m_maze.reserve(COLS * ROWS);
        for (std::size_t row : std::views::iota(0, ROWS)) {
            for (std::size_t col : std::views::iota(0, COLS)) {
                m_maze.emplace_back(col, row);
            }
        }

        m_maze.front().m_visited = true;

        for (auto& cell : m_maze) {
            cell.draw(this);
        }

        m_visitor.push(&m_maze.front());

        return true;
    }

    bool OnUserUpdate(float elapsed_time) override {
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (m_visitor.empty()) {
            m_maze.at(0).draw(this, olc::RED);
            return true;
        }

        for (auto& cell : m_maze) {
            cell.draw(this);
        }

        auto& current_cell = m_visitor.top();
        current_cell->draw(this, olc::GREEN);

        // get neighbours
        auto neighbours = current_cell->get_neighbours();

        // filter for unvisited neighbours
        std::erase_if(neighbours, [&](Direction dir) {
            std::size_t offset_x{0};
            std::size_t offset_y{0};
            switch (dir) {
                case Direction::North:
                    offset_y = -1;
                    break;
                case Direction::East:
                    offset_x = 1;
                    break;
                case Direction::West:
                    offset_x = -1;
                    break;
                case Direction::South:
                    offset_y = 1;
                    break;
                default:
                    break;
            }
            return m_maze.at((current_cell->m_pos.x + offset_x) + (current_cell->m_pos.y + offset_y) * COLS).m_visited;
        });

        // choose a random neighbour
        if (!neighbours.empty()) {

            auto random_neighbour = neighbours.at(rand() % neighbours.size());

            // connect the cells
            switch (random_neighbour) {
                case Direction::North: {
                    auto chosen_neighbour = &m_maze.at((current_cell->m_pos.x) + (current_cell->m_pos.y + -1) * COLS);
                    chosen_neighbour->remove_wall(Wall::South);
                    chosen_neighbour->m_visited = true;
                    m_visitor.push(chosen_neighbour);
                    current_cell->remove_wall(Wall::North);
                } break;
                case Direction::East: {
                    auto chosen_neighbour = &m_maze.at((current_cell->m_pos.x + 1) + (current_cell->m_pos.y) * COLS);
                    chosen_neighbour->remove_wall(Wall::West);
                    chosen_neighbour->m_visited = true;
                    m_visitor.push(chosen_neighbour);
                    current_cell->remove_wall(Wall::East);
                } break;
                case Direction::West: {
                    auto chosen_neighbour = &m_maze.at((current_cell->m_pos.x - 1) + (current_cell->m_pos.y) * COLS);
                    chosen_neighbour->remove_wall(Wall::East);
                    chosen_neighbour->m_visited = true;
                    m_visitor.push(chosen_neighbour);
                    current_cell->remove_wall(Wall::West);
                } break;
                case Direction::South: {
                    auto chosen_neighbour = &m_maze.at((current_cell->m_pos.x) + (current_cell->m_pos.y + 1) * COLS);
                    chosen_neighbour->remove_wall(Wall::North);
                    chosen_neighbour->m_visited = true;
                    m_visitor.push(chosen_neighbour);
                    current_cell->remove_wall(Wall::South);
                } break;
                default:
                    break;
            }
        } else {
            m_visitor.pop();
        }

        return true;
    }

  private:
    std::vector<Cell> m_maze;

    std::stack<Cell*> m_visitor;
};

int main(int argc, char const* argv[]) {
    MazeGenerator maze_generator;
    if (maze_generator.Construct(WINDOW_WIDTH, WINDOW_HIGHT, 4, 4)) {
        maze_generator.Start();
    }
    return 0;
}
