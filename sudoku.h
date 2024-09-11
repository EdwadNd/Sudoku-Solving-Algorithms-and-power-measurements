#ifndef SUDOKU_H
#define SUDOKU_H

#include <iostream>
#include <random>
#include <ctime>
#include <vector>


using std::vector;
enum class Difficulty {
    EASY,
    MEDIUM,
    HARD,
    EXTREME
};

const int GRID_SIZE = 9;

static const int CELLS_TO_REMOVE_EASY = 25;    // Adjust these values
static const int CELLS_TO_REMOVE_MEDIUM = 46;  // based on difficulty
static const int CELLS_TO_REMOVE_HARD = 50;
static const int CELLS_TO_REMOVE_EXTREME = 60;



class Sudoku {
    public:
        Sudoku();
        ~Sudoku() = default;

        void generate(Difficulty difficulty);
        bool solve();
        void draw();    
        void drawSolution();
        bool solve_puzzle_BCktrc( int row, int col);
        bool valid_move(int row, int col, int val);
        clock_t timer, timer2;

    private:
        int CELLS_TO_REMOVE;
        bool n_in_quad(int n, int i, int j);
        bool n_in_col(int n, int j);

        vector<bool> get_quad_possibles(int i, int j);
        vector<bool> get_col_possibles(int j);
        vector<bool> get_row_possibles(int i);

        void set_cell_solved(int i, int j, int n);
        int check_solved_cells();
        void check_hidden_single(int i, int j);

        bool is_solution_right();
        void copy_solution();

        int grid[GRID_SIZE][GRID_SIZE];
        int solution[GRID_SIZE][GRID_SIZE];
        vector<bool> possibles[GRID_SIZE][GRID_SIZE];

        int solution_changes;
        //clock_t timer, timer2 ;

};

#endif // SUDOKU_H