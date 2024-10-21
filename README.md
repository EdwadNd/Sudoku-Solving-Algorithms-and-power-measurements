# Sudoku
## this repo contains four algorithm implemantation : The backtracking algorithm and puzzle generator algorithm 
The algorithms were coded in c++, 

### backtracking algorithm 
The backtracking algorithm is good at solving constraint satisfaction problems by exploring some or all possible solutions [3].
The backtracking algorithm start with searching for an empty cell, from position (0,0). The algorithm adds a number in the empty cell from all possible candidates 1-9. The number must adhere to the sudoku constraints i.e. involves having each digit appear only once on each row, column and 3x3 sub-grid. After filling in the number, the algorithm finds the next empty cell and tries to fill in a valid candidate. However, if there is no valid option, the algorithm backtracks to the previously filled cell and tries to fill in the next possible number or empties it if there is no valid number. The backtracking algorithm guarantees a solution by iterating through all possible solutions. 
### Puzzle generator 
The sudoku puzzles generator uses a backtracking algorithm to generate a sudoku puzzle. The user inputs a seed for randomisation, the backtracking algorithm starts with an empty grid and creates a fully solved sudoku board. The sudoku puzzles is then created by removing a specified number from the fully solved sudoku board. The input seeding is used to randomly select numbers to be removed. this guarantees the creation of different puzzles. The difficulty level is set using the number of empty cells (e.g. 22 cells for easy, 35 cells for medium and 54 cells for hard). Each puzzle is converted to a string representation, separated by a comma of 81 numbers, the puzzles are checked for duplicates before they are saved to a file to ensure each puzzle uniqueness.
### Forward Checking
The forward-checking algorithm initialises the grid by creating legal values for all p
starts by initialising the grid with values for tracking legal values for each cell, the algorithm randomly selects an empty cell, and the legal values are randomised to explore different solution paths. forward-checking ensures that neighbouring cells have valid options by verifying that the assignment does not affect neighbouring cells. However, the algorithm backtracks and updates constraints if there are no valid options. The forward-checking algorithm prunes the search space by reducing dead ends. 
### Forward Checking with Heuristics.
forward-checking with Heuristics uses the same principles as forward-checking except that it uses an intelligent cell selection process. forward-checking with Heuristics uses the Most Constrained Variable (MCV) heuristics to select a cell with the fewest legal options, and it uses the Least Constrained value (LCV) for minimising constraints on neighbouring cells. In situations where there are ties, the algorithm selects the cell that influences most cells other cells. forward-checking with Heuristics efficiently solves the puzzles by reducing dead ends and the need for backtracking. 

### smart PDU Logging
This Node js file uses APIs to get information from the smart PDU. 

