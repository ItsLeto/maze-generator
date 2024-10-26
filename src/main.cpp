#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "cell.hpp"
#include <array>
#include <bitset>
#include <cstdint>
#include <ranges>
#include <stack>

constexpr static auto COLS{30};
constexpr static auto ROWS{20};
constexpr static auto CELL_WIDTH{10};
constexpr static auto CELL_HIGHT{10};
constexpr static auto WALL_WIDTH{1};
constexpr static auto WINDOW_WIDTH{COLS * (CELL_WIDTH + WALL_WIDTH)};
constexpr static auto WINDOW_HIGHT{ROWS * (CELL_WIDTH + WALL_WIDTH)};

class MazeGenerator : public olc::PixelGameEngine {

  public:
    MazeGenerator(std::size_t cols, std::size_t rows, std::size_t cell_width, std::size_t cell_height) :
        m_cols(cols), m_rows(rows) {
        sAppName.assign("MazeGenerator");
        m_grid.reserve(cols * rows);
        for (std::size_t row : std::views::iota(std::size_t(0), m_rows)) {
            for (std::size_t col : std::views::iota(std::size_t(0), m_cols)) {
                m_grid.emplace_back(col, row, cell_width, cell_height);
            }
        }
        cell_at(0, 0).set_visited();
        visitor.push(&cell_at(0, 0));
    }

    bool OnUserCreate() override {
        srand(static_cast<unsigned>(time(0)));
        return true;
    }

    bool OnUserUpdate(float elapsed_time) override {
        if (visitor.empty()) {
            return true;
        }

        draw(this);
        auto& current_cell = visitor.top();
        current_cell->draw(this, olc::GREEN);

        auto unvisited_neighbours = get_unvisited_neighbours(current_cell->m_x, current_cell->m_y);
        if (!unvisited_neighbours.empty()) {
            auto [random_neighbour_direction, random_neighbour] =
                unvisited_neighbours.at(rand() % unvisited_neighbours.size());
            random_neighbour.set_visited();
            visitor.push(&random_neighbour);
            switch (random_neighbour_direction) {
                case Direction::North: {
                    random_neighbour.remove_wall(Direction::South);
                    current_cell->remove_wall(Direction::North);
                } break;
                case Direction::East: {
                    random_neighbour.remove_wall(Direction::West);
                    current_cell->remove_wall(Direction::East);
                } break;
                case Direction::West: {
                    random_neighbour.remove_wall(Direction::East);
                    current_cell->remove_wall(Direction::West);
                } break;
                case Direction::South: {
                    random_neighbour.remove_wall(Direction::North);
                    current_cell->remove_wall(Direction::South);
                } break;
                default:
                    break;
            }
        } else {
            visitor.pop();
        }

        return true;
    }

  private:
    void draw(olc::PixelGameEngine* pge) {
        for (auto& cell : m_grid) {
            cell.draw(pge);
        }
    }

    Cell& cell_at(std::size_t row, std::size_t col) {
        return m_grid.at(index_from(row, col));
    }

    std::vector<std::pair<Direction, Cell&>> get_unvisited_neighbours(std::size_t col, std::size_t row) {
        std::vector<std::pair<Direction, Cell&>> neighbours;
        if (row > 0 && !cell_at(row - 1, col).is_visited()) {
            neighbours.emplace_back(Direction::North, cell_at(row - 1, col));
        }
        if (col < m_cols - 1 && !cell_at(row, col + 1).is_visited()) {
            neighbours.emplace_back(Direction::East, cell_at(row, col + 1));
        }
        if (row < m_rows - 1 && !cell_at(row + 1, col).is_visited()) {
            neighbours.emplace_back(Direction::South, cell_at(row + 1, col));
        }
        if (col > 0 && !cell_at(row, col - 1).is_visited()) {
            neighbours.emplace_back(Direction::West, cell_at(row, col - 1));
        }

        return neighbours;
    }

    std::size_t index_from(std::size_t row, std::size_t col) {
        return col + row * m_cols;
    }

  private:
    std::size_t m_cols;
    std::size_t m_rows;
    std::vector<Cell> m_grid;

    std::stack<Cell*> visitor;
};

int main(int argc, char const* argv[]) {
    MazeGenerator maze_generator(COLS, ROWS, CELL_WIDTH, CELL_HIGHT);
    if (maze_generator.Construct(WINDOW_WIDTH, WINDOW_HIGHT, 4, 4)) {
        maze_generator.Start();
    }
    return 0;
}
