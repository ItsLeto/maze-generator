
#include "olcPixelGameEngine.h"

#include "cell.hpp"
#include <cstdint>
#include <ranges>
#include <stack>
#include <vector>
class Maze {
  public:
    Maze(std::size_t cols, std::size_t rows, std::size_t cell_width, std::size_t cell_height) :
        m_cols(cols), m_rows(rows) {
        m_grid.reserve(cols * rows);
        for (std::size_t row : std::views::iota(std::size_t(0), m_rows)) {
            for (std::size_t col : std::views::iota(std::size_t(0), m_cols)) {
                m_grid.emplace_back(col, row, cell_width, cell_height);
            }
        }
        cell_at(0, 0).set_visited();
    }

    void draw(olc::PixelGameEngine* pge) {
        for (auto& cell : m_grid) {
            cell.draw(pge);
        }
    }

    void generate(olc::PixelGameEngine* pge) {
        std::stack<Cell*> visitor;
        visitor.push(&cell_at(0, 0));

        while (!visitor.empty()) {
            auto& current_cell = visitor.top();
            current_cell->draw(pge, olc::GREEN);

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
        }
    }

  private:
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
};
