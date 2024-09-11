#include "sudoku.h"

typedef std::mt19937 RandGenerator;
RandGenerator rng;

Sudoku::Sudoku() {
    // Clear grid memory
    for(int i = 0; i < GRID_SIZE; ++i)
        for(int j = 0; j < GRID_SIZE; ++j) {
            grid[i][j] = 0;
            solution[i][j] = 0;
        }
    // Populate random number generator seed
    rng.seed(std::time(nullptr));
}

bool Sudoku::n_in_quad(int n, int i, int j) {
    // translate quad coords to center position in grid
    int center[2] = {i*3 + 1, j*3 + 1};
    // iterate over quad to check if n exists in it
    for(int x = center[0]-1; x <= center[0]+1; ++x) {
        for(int y = center[1]-1; y <= center[1]+1; ++y) {
            if(solution[x][y] == 0) return false;
            if(n == solution[x][y]) return true;
        }
    }
    return false;
}
bool Sudoku::solve_puzzle_BCktrc( int row, int col){
   
    timer2 = clock() - timer;
    if (col == 9) {
        if (row == 8) {
            return true; // Puzzle solved
        }
        row++;
        col = 0;
    }

    if (grid[row][col] > 0) {
       
        return solve_puzzle_BCktrc( row, col + 1);

    }

    for (int i = 1; i <= 9; i++) {
        if (valid_move( row, col, i)) {
            grid[row][col] = i;
            if (solve_puzzle_BCktrc( row, col + 1)) {
                return true;
            }
            grid[row][col] = 0;
        }
    }
     
   // timer = clock();
    return false;
}
bool Sudoku::valid_move(int row, int col, int val){
    // valid row
    for (int i = 0; i < 9; i++) {
        if (grid[row][i] == val) {
            return false;
        }
    }

    // valid column
    for (int i = 0; i < 9; i++) {
        if (grid[i][col] == val) {
            return false;
        }
    }

    // valid square
    int r = row - row % 3;
    int c = col - col % 3;
    for (int i = 0; i < 3; i++) {       for (int j = 0; j < 3; j++) {
            if (grid[r+i][c+j] == val) {
                return false;
            }
        }
    }

    return true;
}


bool Sudoku::n_in_col(int n, int j) {
    // iterate over the col to check if n exists in it
    for(int i = 0; i < GRID_SIZE; ++i) {
        if(solution[i][j] == 0) break;
        if(solution[i][j] == n) return true;
    }
    return false;
}

vector<bool> Sudoku::get_quad_possibles(int i, int j) {
    vector<bool> output(9, true);
    // translate quad coords to center position in grid
    int center[2] = {i*3 + 1, j*3 + 1};
    // iterate over quad to check if n exists in it
    for(int x = center[0]-1; x <= center[0]+1; ++x) {
        for(int y = center[1]-1; y <= center[1]+1; ++y) {
            if(this->grid[x][y] != 0)
                output[this->grid[x][y]-1] = false;
        }
    }
    return output;
}

vector<bool> Sudoku::get_col_possibles(int j) {
    vector<bool> output(9, true);
    for(int i = 0; i < GRID_SIZE; ++i) {
        if(this->grid[i][j] != 0)
            output[this->grid[i][j]-1] = false;
    }
    return output;
}

vector<bool> Sudoku::get_row_possibles(int i) {
    vector<bool> output(9, true);
    for(int j = 0; j < GRID_SIZE; ++j) {
        if(this->grid[i][j] != 0)
            output[this->grid[i][j]-1] = false;
    }
    return output;
}

int possiblesCount(vector<bool> possibles) {
    int count = 0;
    for(int i = 0; i < possibles.size(); ++i) {
        if(possibles[i]) ++count;
    }
    return count;
}

void Sudoku::generate(Difficulty difficulty) {

    // Define the number of cells to remove based on the difficulty
    int cellsToRemove;
    switch (difficulty) {
        case Difficulty::EASY:
            CELLS_TO_REMOVE = CELLS_TO_REMOVE_EASY;
            break;
        case Difficulty::MEDIUM:
            CELLS_TO_REMOVE = CELLS_TO_REMOVE_MEDIUM;
            break;
        case Difficulty::HARD:
            CELLS_TO_REMOVE= CELLS_TO_REMOVE_HARD;
            break;
        case Difficulty::EXTREME:
            CELLS_TO_REMOVE = CELLS_TO_REMOVE_EXTREME;
            break;
    }

    vector<bool> oneToNine(9, true);
     timer = clock();
    // Generate grid
    for(int i = 0; i < GRID_SIZE; ++i) {
        // Generate horizontal possibles
        vector<bool> h_possibles = oneToNine;

        int j = 0;
        while(j < GRID_SIZE) {          
            // If not initialized yet store the possibles of the row in that pos
            if(this->possibles[i][j].size() == 0) {
                this->possibles[i][j] = h_possibles;             
            }

            // Check if we are out of possible numbers
            if(possiblesCount(this->possibles[i][j]) == 0) {   
                // Replenish this square numbers
                this->possibles[i][j].resize(0);

                --j; // Go back 1 square
                // Remove the chosen one from the possibles
                this->possibles[i][j][solution[i][j]-1] = false;
                h_possibles[solution[i][j]-1] = true;
                solution[i][j] = 0;
            } else {
                // Get a number randomly from the available numbers
                std::uniform_int_distribution<uint32_t> tempGenerator(0, possiblesCount(this->possibles[i][j])-1);                
                int n = tempGenerator(rng)+1;
                int choosen = 0;
                while(true) { // Choose from the possible ones the n-one
                    if(this->possibles[i][j][choosen]) --n;
                    if(n == 0) break;
                    ++choosen;
                }
                

                // Check if it conflicts
                if(n_in_quad(choosen+1, i/3, j/3) || n_in_col(choosen+1, j)) {
                    // Remove from available numbers for this square
                    this->possibles[i][j][choosen] = false;
                } else {
                    // Use it and go forward
                    solution[i][j] = choosen+1;
                    h_possibles[choosen] = false;
                    ++j;        
                }             
            }
        }
    }
    //timer = clock();

    // remove from grid according to the only-one-solution rule
    copy_solution();
}

// ------- Solving functions -------

// n is 0-based
void Sudoku::set_cell_solved(int i, int j, int n) {
    int center[2] = {(i/3)*3 + 1, (j/3)*3 + 1};
    // iterate over quad to remove n from possibles
    for(int x = center[0]-1; x <= center[0]+1; ++x) {
        for(int y = center[1]-1; y <= center[1]+1; ++y) {
            this->possibles[x][y][n] = false;
        }
    }

    // Vertical and horizontal removal
    for(int x = 0; x < GRID_SIZE; ++x) {
        this->possibles[x][j][n] = false;
        this->possibles[i][x][n] = false;
    }

    this->solution_changes++;
    this->grid[i][j] = n+1;
}

int Sudoku::check_solved_cells() {
    int solved = 0;
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            if(this->grid[i][j] == 0 && 
                possiblesCount(this->possibles[i][j]) == 1) {
                // Get the only possible one
                for(int x = 0; x < 9; ++x) {
                    if(this->possibles[i][j][x]) {
                        solved++;
                        set_cell_solved(i, j, x);
                        break;
                    }
                }             
            }
        }
    }
    return solved;
}

// Last Remaining Cell in a Box
void Sudoku::check_hidden_single(int i, int j) {
    // Get quad center coords
    int center[2] = {(i/3)*3 + 1, (j/3)*3 + 1};
    
    // Check for repeated in quad
    int times[GRID_SIZE] = {0};
    for(int x = center[0]-1; x <= center[0]+1; ++x)
        for(int y = center[1]-1; y <= center[1]+1; ++y)
            if(x != i && y != j) { // When cell is different
                // Count the possibles
                for(int n = 0; n < 9; ++n) {
                    if(this->possibles[x][y][n]) times[n]++;
                }              
            }
    for(int n = 0; n < GRID_SIZE; ++n) {
        if(times[n] == 1) {
            // Only one possible n in quad
            set_cell_solved(i, j, n);
        }
    }

    // Check for repeated in row and col
    std::fill_n(times, GRID_SIZE, 0); // reset times array
    int times_col[GRID_SIZE] = {0};
    for(int x = 0; x < GRID_SIZE; ++x) {
        for(int n = 0; n < 9; ++n) {
            // Count the possibles of the row
            if(this->possibles[i][x][n]) times[n]++;
            // Count the possibles of the col
            if(this->possibles[x][j][n]) times_col[n]++;
        }              
    }
    for(int n = 0; n < GRID_SIZE; ++n) {
        if(times[n] == 1) {
            // Only one possible n in row
            set_cell_solved(i, j, n);
        } else if(times_col[n] == 1) {
            // Only one possible n in col
            set_cell_solved(i, j, n);
        }
    }
}

bool Sudoku::solve() {
    this->solution_changes = 0;
    // clear memory from possibles array
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            this->possibles[i][j] = vector<bool>(9, false);
        }
    }

    while(true) {
        int lastChanges = this->solution_changes;
        // Get absolute vertical possibilities
        vector<bool> v_possibles[GRID_SIZE];
        vector<bool> h_possibles[GRID_SIZE];
        for(int j = 0; j < GRID_SIZE; ++j) {
            v_possibles[j] = get_col_possibles(j);         
        }

        // Calculate all possibles of each cell
        for(int i = 0; i < GRID_SIZE; ++i) {
            h_possibles[i] = get_row_possibles(i);
            for(int j = 0; j < GRID_SIZE; ++j) {
                vector<bool> q_possibles = get_quad_possibles(i/3, j/3);
                for(int n = 0; n < 9; ++n) {
                    this->possibles[i][j][n] = (h_possibles[i][n] && 
                                                v_possibles[j][n] && 
                                                q_possibles[n]);
                }
            }
        }

        // Check for solved cells
        if(check_solved_cells() == 0) {
            // Other strategies
            for(int i = 0; i < GRID_SIZE; ++i) {
                for(int j = 0; j < GRID_SIZE; ++j) {
                    if(this->grid[i][j] != 0) continue; 
                    check_hidden_single(i, j);
                }
            }
        }

        // Avoid infinite loops
        if(lastChanges == this->solution_changes) break;
    }

    return is_solution_right();
}

void Sudoku::copy_solution() {
    // To store the removed cells coordinates
    int x_removed[CELLS_TO_REMOVE];
    int y_removed[CELLS_TO_REMOVE];

    while(true) { // while more than one solution
        for(int i = 0; i < GRID_SIZE; ++i) {
            for(int j = 0; j < GRID_SIZE; ++j) {
                this->grid[i][j] = this->solution[i][j];
            }
        }
        
        // I should do a backtracking way of removing cells so
        // it has only a solution but i just need a simple generator
        std::uniform_int_distribution<uint32_t> tempGenerator(0, 8);
        for(int i = 0; i < CELLS_TO_REMOVE; ++i) {
            x_removed[i] = tempGenerator(rng);
            y_removed[i] = tempGenerator(rng);
            // Avoid repeated cells
            while(this->grid[x_removed[i]][y_removed[i]] == 0) {
                x_removed[i] = tempGenerator(rng);
                y_removed[i] = tempGenerator(rng);
            }            
            this->grid[x_removed[i]][y_removed[i]] = 0;
        }
        if(this->solve()) break; // End loop when found only one solution sudoku
    }

    for(int i = 0; i < CELLS_TO_REMOVE; ++i) {
        this->grid[x_removed[i]][y_removed[i]] = 0;
    }
}

bool Sudoku::is_solution_right() {
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            if(this->grid[i][j] != this->solution[i][j]) {                
                return false;
            }
        }
    }
    return true;
}

// ---------------------------------
// ------- Drawing functions -------
// ---------------------------------
void draw_separator() { 
    std::cout << "+-------+-------+-------+" << std::endl; 
}

char draw_cell_content(const int n) {
    // if cell holds a 0 (undefined number), draw a '.' instead
    if(n == 0) return '.';
    else return n + '0';
}

void drawSudoku(int grid[][GRID_SIZE]) {
    for(int i = 0; i < GRID_SIZE; ++i) {
        if(i % 3 == 0) draw_separator();
        for(int j = 0; j < GRID_SIZE; ++j) {
            // draw col separator
            if (j % 3 == 0) std::cout << (!j ? "" : " ") << "|";
            std::cout << " " << draw_cell_content(grid[i][j]);
        }
        std::cout << " |" << std::endl;
    }
    draw_separator();
}

void Sudoku::draw() {
    drawSudoku(this->grid);
    
}

void Sudoku::drawSolution() {
    drawSudoku(this->solution);
}