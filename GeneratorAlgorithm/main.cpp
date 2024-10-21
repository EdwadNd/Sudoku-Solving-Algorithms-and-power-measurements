#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <set>

using namespace std;

const int SIZE = 9;
const int TOTAL_BOARDS = 232258;

// Function to check if placing num at board[row][col] is valid
bool isValid(const vector<vector<int>>& board, int row, int col, int num) {
    // Check if num is already in the row
    for (int x = 0; x < SIZE; x++) {
        if (board[row][x] == num) {
            return false;
        }
    }

    // Check if num is already in the column
    for (int x = 0; x < SIZE; x++) {
        if (board[x][col] == num) {
            return false;
        }
    }

    // Check if num is in the 3x3 sub-grid
    int startRow = row / 3 * 3;
    int startCol = col / 3 * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[startRow + i][startCol + j] == num) {
                return false;
            }
        }
    }

    return true;
}

// Function to solve the board using backtracking
bool solve(vector<vector<int>>& board) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (board[row][col] == 0) { // Find an empty cell
                for (int num = 1; num <= SIZE; num++) {
                    if (isValid(board, row, col, num)) {
                        board[row][col] = num;
                        if (solve(board)) {
                            return true;
                        }
                        board[row][col] = 0; // Backtrack
                    }
                }
                return false; // No valid number found, trigger backtracking
            }
        }
    }
    return true; // Solved
}

// Function to generate a fully solved Sudoku board
vector<vector<int>> generateCompleteBoard() {
    vector<vector<int>> board(SIZE, vector<int>(SIZE, 0));
    solve(board);
    return board;
}

// Function to remove numbers from the board to create a puzzle
void removeNumbers(vector<vector<int>>& board, int numCellsToRemove) {
    int removed = 0;
    while (removed < numCellsToRemove) {
        int row = rand() % SIZE;
        int col = rand() % SIZE;

        if (board[row][col] != 0) {
            board[row][col] = 0;
            removed++;
        }
    }
}

// Function to generate a Sudoku puzzle with a specific number of empty cells
vector<vector<int>> generate(int numCellsToRemove) {
    // Step 1: Generate a fully solved Sudoku board
    vector<vector<int>> completeBoard = generateCompleteBoard();

    // Step 2: Remove numbers to create the puzzle
    removeNumbers(completeBoard, numCellsToRemove);

    return completeBoard;
}



// Function to convert the board to a single-line string with comma-separated values
string boardToCSVString(const vector<vector<int>>& board) {
    string boardStr;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            boardStr += to_string(board[i][j]);
            if (i != SIZE - 1 || j != SIZE - 1) {
                boardStr += ",";
            }
        }
    }
    return boardStr;
}

// Custom seeding function using system clock and user input
void customSeed(int userInput) {
    auto now = chrono::high_resolution_clock::now();
    long long clockSeed = now.time_since_epoch().count();
    srand(static_cast<unsigned int>(clockSeed + userInput));
}

int main() {
    string filename = "medium32n.txt";
    ofstream outfile(filename);

    int userSeed;
    cout << "Enter a seed value (integer): ";
    cin >> userSeed;

    // Set custom seed using system clock and user input
    customSeed(userSeed);

    set<string> uniqueBoards; // Set to store unique boards as strings
    int boardsGenerated = 0;
    int numCellsToRemove = 22; // Example for medium difficulty

    while (boardsGenerated < TOTAL_BOARDS) {
        vector<vector<int>> sudokuBoard = generate(numCellsToRemove);
        string boardStr = boardToCSVString(sudokuBoard);

        // Check if the board is unique
        if (uniqueBoards.find(boardStr) == uniqueBoards.end()) {
            // If unique, save to the set and file
            uniqueBoards.insert(boardStr);
            outfile << boardStr << endl;
            boardsGenerated++;

            // Progress message
            cout << "Generated board #" << boardsGenerated << " / " << TOTAL_BOARDS << endl;
        } else {
            // Regenerate if the board is a duplicate
            cout << "Duplicate board detected. Regenerating..." << endl;
        }
    }

    outfile.close();
    cout << "All " << TOTAL_BOARDS << " boards generated and saved to " << filename << endl;

    return 0;
}
