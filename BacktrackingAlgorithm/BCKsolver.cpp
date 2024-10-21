#include <iostream>
#include <array>
#include <chrono>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <psapi.h>
#include <thread>
#include <iomanip>
#include <sys/stat.h>

using namespace std;
using namespace std::chrono;

array<array<int, 9>, 9> puzzle;

void print_puzzle(const array<array<int, 9>, 9>& puzzle);
bool valid_move(const array<array<int, 9>, 9>& puzzle, int row, int col, int val);
bool solve_puzzle(array<array<int, 9>, 9>& puzzle, int row, int col);
bool read_next_puzzle(ifstream& file);

bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Helper function to write a formatted row to the CSV file
void write_csv_row(ofstream& file, const string& col1, const string& col2, const string& col3, const string& col4) {
    file << "| " << setw(10) << left << col1 << " | "
         << setw(20) << left << col2 << " | "
         << setw(20) << left << col3 << " | "
         << setw(25) << left << col4 << " |" << endl;
}

int main() {
    const string log_filename = "hard_log2.csv";
    bool log_exists = file_exists(log_filename);
    ofstream logFile(log_filename, ios::app); // Open in append mode
    
    if (!logFile.is_open()) {
        cout << "Unable to open log file: " << log_filename << endl;
        return 1;
    }

    // If the log file didn't exist before, write the header
    if (!log_exists) {
        write_csv_row(logFile, "Puzzle", "Elapsed Time (s)", "CPU Time (s)", "Peak Memory Usage (MB)");
        logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;
    }

    ifstream file("Hard54.txt");
    if (!file.is_open()) {
        cout << "Unable to open file: " << endl;
        return 1;
    }

    double total_elapsed_time = 0.0;
    double total_cpu_time = 0.0;
    SIZE_T peak_memory_usage = 0;
    int boardCount = 1;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    while (read_next_puzzle(file)) {
        LARGE_INTEGER start_time, end_time;
        QueryPerformanceCounter(&start_time);

        // Get initial CPU usage time
        FILETIME creationTime, exitTime, kernelTime, userTime;
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        ULARGE_INTEGER startUserTime;
        startUserTime.LowPart = userTime.dwLowDateTime;
        startUserTime.HighPart = userTime.dwHighDateTime;

        cout << "\nOriginal Puzzle #" << boardCount << ":";
        print_puzzle(puzzle);

        if (solve_puzzle(puzzle, 0, 0)) {
            cout << "\nThe puzzle is solved:";
            print_puzzle(puzzle);
        } else {
            cout << "\nThis puzzle is not Solvable\n";
        }

        QueryPerformanceCounter(&end_time);
        double elapsed_time = static_cast<double>(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
        total_elapsed_time += elapsed_time;

        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        ULARGE_INTEGER endUserTime;
        endUserTime.LowPart = userTime.dwLowDateTime;
        endUserTime.HighPart = userTime.dwHighDateTime;

        double cpu_time = (endUserTime.QuadPart - startUserTime.QuadPart) / 1e7;
        total_cpu_time += cpu_time;

        PROCESS_MEMORY_COUNTERS_EX pmc;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
        SIZE_T peakMemUsedByMe = pmc.PeakWorkingSetSize;
        peak_memory_usage = max(peak_memory_usage, peakMemUsedByMe);

        // Log performance data to CSV in a table-like format
        write_csv_row(logFile, 
                      to_string(boardCount), 
                      to_string(elapsed_time), 
                      to_string(cpu_time), 
                      to_string(peakMemUsedByMe / (1024.0 * 1024.0)));

        cout << "Puzzle " << boardCount << " solved in " << elapsed_time << " seconds." << endl;
        cout << "CPU Time: " << cpu_time << " seconds" << endl;
        cout << "Peak Memory Usage: " << peakMemUsedByMe / (1024 * 1024) << " MB" << endl;

        boardCount++;
    }

    // Print and log the total metrics for all boards
    cout << "\nTotal time taken for all boards: " << total_elapsed_time << " seconds" << endl;
    cout << "Total CPU time for all boards: " << total_cpu_time << " seconds" << endl;
    cout << "Peak memory usage across all boards: " << peak_memory_usage / (1024 * 1024) << " MB" << endl;

    logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;
    write_csv_row(logFile, "Total", to_string(total_elapsed_time), to_string(total_cpu_time), to_string(peak_memory_usage / (1024.0 * 1024.0)));
    
    logFile.close();
    file.close();
    int x=0;
    cin >> x;
    return 0;
}

// Function to read the next puzzle from the file
bool read_next_puzzle(ifstream& file) {
    string line;
    if (!getline(file, line)) {
        return false;  // No more puzzles to read
    }

    istringstream iss(line);
    string value;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (!getline(iss, value, ',')) {
                cout << "Not enough values in line " << i + 1 << endl;
                return false;
            }
            puzzle[i][j] = stoi(value);
        }
    }
    return true;
}

bool solve_puzzle(array<array<int, 9>, 9>& puzzle, int row, int col) {
    if (col == 9) {
        if (row == 8) {
            return true;  // Puzzle solved
        }
        row++;
        col = 0;
    }

    if (puzzle[row][col] > 0) {
       // print_puzzle(puzzle);
      //  this_thread::sleep_for(chrono::seconds(1));
        return solve_puzzle(puzzle, row, col + 1);
    }

    for (int i = 1; i <= 9; i++) {
        if (valid_move(puzzle, row, col, i)) {
            puzzle[row][col] = i;
           // print_puzzle(puzzle);
            if (solve_puzzle(puzzle, row, col + 1)) {
                return true;
            }
            puzzle[row][col] = 0;
        }
    }
    return false;
}

bool valid_move(const array<array<int, 9>, 9>& puzzle, int row, int col, int val) {
    // valid row
    for (int i = 0; i < 9; i++) {
        if (puzzle[row][i] == val) {
            return false;
        }
    }

    // valid column
    for (int i = 0; i < 9; i++) {
        if (puzzle[i][col] == val) {
            return false;
        }
    }

    // valid square
    int r = row - row % 3;
    int c = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (puzzle[r + i][c + j] == val) {
                return false;
            }
        }
    }

    return true;
}

void print_puzzle(const array<array<int, 9>, 9>& puzzle) {
    cout << "\n+-------+-------+-------+";
    for (int row = 0; row < 9; row++) {
        if (row % 3 == 0 && row != 0) {
            cout << "\n|-------+-------+-------|";
        }
        cout << "\n";
        for (int col = 0; col < 9; col++) {
            if (col % 3 == 0) {
                cout << "| ";
            }
            if (puzzle[row][col] != 0) {
                cout << puzzle[row][col] << " ";
            } else {
                cout << "  ";
            }
        }
        cout << "|";
    }
    cout << "\n+-------+-------+-------+\n";
}
