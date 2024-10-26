#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "cell.hpp"
#include <cmath>
#include <array>
#include <bitset>
#include <cstdint>
#include <cstdlib>
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
    MazeGenerator(int32_t cols, int32_t rows, int32_t cell_width, int32_t cell_height) :
        m_cols(cols), m_rows(rows), m_goal(7, 20, cell_width, cell_height) {
        sAppName.assign("MazeGenerator");
        m_grid.reserve(cols * rows);
        for (int32_t row : std::views::iota(int32_t(0), m_rows)) {
            for (int32_t col : std::views::iota(int32_t(0), m_cols)) {
                m_grid.emplace_back(col, row, cell_width, cell_height);
            }
        }
        cell_at(0, 0).set_visited();

        visitor.push(&cell_at(0, 0));

        open_set.push_back(&cell_at(0, 0));
        m_goal = m_grid.at(rand() % m_grid.size());
        m_goal.set_visited();
        cell_at(0, 0).m_f_score = heuristic(cell_at(0, 0), m_goal);
    }

    bool OnUserCreate() override {
        srand(static_cast<unsigned>(time(0)));
        return true;
    }

    bool OnUserUpdate(float elapsed_time) override {
        if (!visitor.empty()) {
            // generate maze
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
        } else {
            // solve maze
            auto score = [&](Cell* cell) {
                return std::abs(cell->m_x - m_goal.m_x) * std::abs(cell->m_y - m_goal.m_y);
            };

            if (!open_set.empty()) {
                auto current_best_cell = open_set.front();
                current_best_cell->draw(this, olc::MAGENTA);
                m_goal.draw(this, olc::RED);
                if (current_best_cell->m_x == m_goal.m_x && current_best_cell->m_y == m_goal.m_y) {
                    return true;
                }

                std::ranges::pop_heap(open_set);
                open_set.pop_back();
                auto neighbours = get_neighbours(current_best_cell->m_x, current_best_cell->m_y);
                for (auto& neighbour : neighbours) {
                    // double dist = olc::v_2d<int32_t>(current_best_cell->m_x - neighbour.m_x,
                    //                                      current_best_cell->m_y - neighbour.m_y)
                    //                   .mag();
                    auto tentative_gScore = current_best_cell->m_g_score + 1;
                    if (tentative_gScore < neighbour->m_g_score) {

                        // cameFrom[neighbor] := current;
                        neighbour->m_g_score = tentative_gScore;
                        neighbour->m_f_score = tentative_gScore + heuristic(*neighbour, m_goal);

                        if (open_set.end() == std::ranges::find_if(open_set, [&](Cell*& cell) {
                                return cell->m_x == neighbour->m_x && cell->m_y == neighbour->m_y;
                            })) {

                            open_set.push_back(neighbour);
                            std::ranges::push_heap(open_set, [&](auto c1, auto c2) { return score(c1) < score(c2); });
                        }
                    }

                    // if neighbor not in openSet
                    //      openSet.add(neighbor)
                }
            }
        }

        return true;
    }

  private:
    void draw(olc::PixelGameEngine* pge) {
        for (auto& cell : m_grid) {
            cell.draw(pge);
        }
    }

    Cell& cell_at(int32_t row, int32_t col) {
        return m_grid.at(index_from(row, col));
    }

    std::vector<Cell*> get_neighbours(int32_t col, int32_t row) {
        std::vector<Cell*> neighbours;
        auto& cell = cell_at(row, col);
        if (!cell.has_wall(Direction::North)) {
            neighbours.emplace_back(&cell_at(row - 1, col));
        }
        if (!cell.has_wall(Direction::East)) {
            neighbours.emplace_back(&cell_at(row, col + 1));
        }
        if (!cell.has_wall(Direction::South)) {
            neighbours.emplace_back(&cell_at(row + 1, col));
        }
        if (!cell.has_wall(Direction::West)) {
            neighbours.emplace_back(&cell_at(row, col - 1));
        }

        return neighbours;
    }

    std::vector<std::pair<Direction, Cell&>> get_unvisited_neighbours(int32_t col, int32_t row) {
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

    int32_t index_from(int32_t row, int32_t col) {
        return col + row * m_cols;
    }

    double heuristic(Cell& cell, Cell& goal) {
        return std::abs(cell.m_x - goal.m_x) * std::abs(cell.m_y - goal.m_y);
    }

  private:
    int32_t m_cols;
    int32_t m_rows;
    std::vector<Cell> m_grid;

    // generating the maze
    std::stack<Cell*> visitor;

    // solving the maze
    std::vector<Cell*> open_set;
    Cell m_goal;
};

int main(int argc, char const* argv[]) {
    MazeGenerator maze_generator(COLS, ROWS, CELL_WIDTH, CELL_HIGHT);
    if (maze_generator.Construct(WINDOW_WIDTH, WINDOW_HIGHT, 4, 4)) {
        maze_generator.Start();
    }
    return 0;
}
