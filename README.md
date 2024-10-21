# Sudoku
## this repo contains two algorithm implemantation : The backtracking algorithm and puzzle generator algorithm 
The algorithms were coded in c++, 

### backtracking algorithm 
The backtracking algorithm is good at solving constraint satisfaction problems by exploring some or all possible solutions [3].
The backtracking algorithm start with searching for an empty cell, from position (0,0). The algorithm adds a number in the empty cell from all possible candidates 1-9. The number must adhere to the sudoku constraints i.e. involves having each digit appear only once on each row, column and 3x3 sub-grid. After filling in the number, the algorithm finds the next empty cell and tries to fill in a valid candidate. However, if there is no valid option, the algorithm backtracks to the previously filled cell and tries to fill in the next possible number or empties it if there is no valid number. The backtracking algorithm guarantees a solution by iterating through all possible solutions. 
### Puzzle generator 
The sudoku puzzles generator uses a backtracking algorithm to generate a sudoku puzzle. The user inputs a seed for randomisation, the backtracking algorithm starts with an empty grid and creates a fully solved sudoku board. The sudoku puzzles is then created by removing a specified number from the fully solved sudoku board. The input seeding is used to randomly select numbers to be removed. this guarantees the creation of different puzzles. The difficulty level is set using the number of empty cells (e.g. 22 cells for easy, 35 cells for medium and 54 cells for hard). Each puzzle is converted to a string representation, separated by a comma of 81 numbers, the puzzles are checked for duplicates before they are saved to a file to ensure each puzzle uniqueness.
### smart PDU Logging
This Node js file uses APIs to get information from the smart PDU. 
