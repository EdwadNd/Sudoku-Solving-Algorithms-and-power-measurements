#include <utility>
#include <ctime>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include "AbstractSudokuSolver.h"
#include "ConstraintProblem.h"
#include "SudokuSolver_Basic.h"
#include "SudokuSolver_ForwardChecking.h"
#include "SudokuSolver_FC_Heuristics.h"

using namespace std;

// Helper function to map vector to a grid structure
map<PairIndex, int> list2map(const vector<int>& v) {
    map<PairIndex, int> sudokuMap;
    int count = 0;
    for (int i : v) {
        sudokuMap[PairIndex(count / 9, count % 9)] = i;
        count++;
    }
    return sudokuMap;
}

// Helper function to check if the file exists
bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Helper function to write a row in CSV format
void write_csv_row(ofstream& file, const string& col1, const string& col2, const string& col3, const string& col4) {
    file << "| " << setw(10) << left << col1 << " | "
         << setw(20) << left << col2 << " | "
         << setw(20) << left << col3 << " | "
         << setw(25) << left << col4 << " |" << endl;
}

// Helper function to split a string by a delimiter
vector<string> split_string(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Helper function to convert a string of digits into a vector of integers
vector<int> string_to_puzzle_vector(const string& puzzle_str) {
    vector<int> puzzle;
    for (char c : puzzle_str) {
        if (isdigit(c)) {
            puzzle.push_back(c - '0');  // Convert char to integer
        }
    }
    return puzzle;
}

int main() {
    const string puzzle_file = "HardTexsst.txt";
    ifstream puzzleFile(puzzle_file);

    if (!puzzleFile.is_open()) {
        cout << "Unable to open puzzle file: " << puzzle_file << endl;
        return 1;
    }

    const string log_filename = "Hard54.csv";
    bool log_exists = file_exists(log_filename);
    ofstream logFile(log_filename, ios::app);  // Open in append mode

    if (!logFile.is_open()) {
        cout << "Unable to open log file: " << log_filename << endl;
        return 1;
    }

    // Write CSV header if file didn't exist
    if (!log_exists) {
        write_csv_row(logFile, "Puzzle", "Elapsed Time (s)", "CPU Time (s)", "Peak Memory Usage (MB)");
        logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;
    }

    // Read the entire file as a single string
    string file_contents;
    getline(puzzleFile, file_contents, '\0');  // Read entire file into a string

    // Split the file contents by newlines (assuming there's only one line)
    vector<string> puzzles = split_string(file_contents, '\n');

    int puzzle_count = 0;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    FILETIME creationTime, exitTime, kernelTime, userTime;
    ULARGE_INTEGER startUserTime, endUserTime;

    SIZE_T peak_memory_usage = 0;
    SIZE_T total_memory_usage = 0;
    double total_elapsed_time = 0.0;
    double total_cpu_time = 0.0;

    PROCESS_MEMORY_COUNTERS_EX memInfo;

    // Iterate through each puzzle string
    for (const string& puzzle_str : puzzles) {
        // Split puzzle string by commas
        vector<string> puzzle_digits = split_string(puzzle_str, ',');

        // Ensure the puzzle has exactly 81 digits
        if (puzzle_digits.size() != 81) {
            cout << "Invalid puzzle size: " << puzzle_digits.size() << " (Expected 81 digits)." << endl;
            continue;
        }

        puzzle_count++;

        // Convert the puzzle string into a vector of integers
        vector<int> puzzle;
        for (const string& digit : puzzle_digits) {
            puzzle.push_back(stoi(digit));  // Convert string to integer
        }

        // Convert the puzzle vector into a map format for the solver
        map<PairIndex, int> initialState = list2map(puzzle);

        // Use a solver with Forward Checking and Heuristics
        ConstraintProblem<PairIndex, int>* solver = new SudokuSolver_FC_Heuristics(initialState);
        //ConstraintProblem<PairIndex, int>* solver = new SudokuSolver_ForwardChecking(initialState);

        // Start measuring performance
        LARGE_INTEGER start_time, end_time;
        QueryPerformanceCounter(&start_time);
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        startUserTime.LowPart = userTime.dwLowDateTime;
        startUserTime.HighPart = userTime.dwHighDateTime;

        // Solve the puzzle
        solver->backtrackingSearch();
        solver->printResult();

        // End measuring performance
        QueryPerformanceCounter(&end_time);
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        endUserTime.LowPart = userTime.dwLowDateTime;
        endUserTime.HighPart = userTime.dwHighDateTime;

        // Calculate the elapsed time and CPU time
        double elapsed_time = static_cast<double>(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
        double cpu_time = (endUserTime.QuadPart - startUserTime.QuadPart) / 1e7;

        // Get peak memory usage
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo));
        peak_memory_usage = max(peak_memory_usage, memInfo.PeakWorkingSetSize);

        // Accumulate totals
        total_elapsed_time += elapsed_time;
        total_cpu_time += cpu_time;
        total_memory_usage = max(total_memory_usage, memInfo.PeakWorkingSetSize);

        // Log the performance data for the current puzzle
        write_csv_row(logFile, "Puzzle " + to_string(puzzle_count), to_string(elapsed_time), to_string(cpu_time), to_string(peak_memory_usage / (1024.0 * 1024.0)));

        // Clean up solver
        delete solver;

        // Output to console
        cout << "Puzzle #" << puzzle_count << " solved." << endl;
        cout << "Elapsed Time: " << elapsed_time << " seconds" << endl;
        cout << "CPU Time: " << cpu_time << " seconds" << endl;
        cout << "Peak Memory Usage: " << peak_memory_usage / (1024.0 * 1024.0) << " MB" << endl;
    }

    // Write totals to the log file
    logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;
    write_csv_row(logFile, "TOTALS", to_string(total_elapsed_time), to_string(total_cpu_time), to_string(total_memory_usage / (1024.0 * 1024.0)));
    logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;

    logFile.close();
    puzzleFile.close();

    return 0;
}
