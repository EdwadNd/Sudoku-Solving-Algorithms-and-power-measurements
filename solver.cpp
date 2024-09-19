#include <bits/stdc++.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <sys/stat.h>

using namespace std;

// Seed for PRNG
int SEED = 42;
mt19937 rng(SEED);

int randomInt(int low, int high) {
    uniform_int_distribution<> dist(low, high);
    return dist(rng);
}

double randomDouble() {
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int getCost(vector<vector<int>>& grid) {
    int cost = 0;
    for (int i = 0; i < 9; i++) {
        set<int> rowSet, columnSet;
        for (int j = 0; j < 9; j++) {
            rowSet.insert(grid[i][j]); // Distinct elements in each row
            columnSet.insert(grid[j][i]); // Distinct elements in each column
        }
        cost += (18 - (rowSet.size() + columnSet.size()));
    }
    return cost;
}

void printGrid(vector<vector<int>>& grid) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void populateGrid(vector<vector<int>>& grid, vector<vector<bool>>& fixed) {
    for (int rowBox = 0; rowBox < 3; rowBox++) {
        for (int colBox = 0; colBox < 3; colBox++) { // Go over each 3x3 box
            set<int> boxSet;
            for (int i = 1; i < 10; i++) boxSet.insert(i);

            for (int i = 3 * rowBox; i < 3 * (rowBox + 1); i++) {
                for (int j = 3 * colBox; j < 3 * (colBox + 1); j++) { // Maintain unfixed elements in each box
                    if (fixed[i][j]) boxSet.erase(grid[i][j]);
                }
            }

            vector<int> boxUnfixed;
            for (int i : boxSet) boxUnfixed.push_back(i);
            shuffle(boxUnfixed.begin(), boxUnfixed.end(), rng); // Shuffle unfixed elements

            int k = 0;
            for (int i = 3 * rowBox; i < 3 * (rowBox + 1); i++) {
                for (int j = 3 * colBox; j < 3 * (colBox + 1); j++) {
                    if (!fixed[i][j]) {
                        grid[i][j] = boxUnfixed[k]; // Put in shuffled elements at empty places
                        k++;
                    }
                }
            }
        }
    }
}

void swapUnfixed(vector<vector<int>>& grid, vector<vector<bool>>& fixed) {
    int rowBox = randomInt(0, 2);
    int colBox = randomInt(0, 2);
    int i1 = 0, j1 = 0;
    while (1) { // Choose first cell
        i1 = 3 * rowBox + randomInt(0, 2);
        j1 = 3 * colBox + randomInt(0, 2);
        if (!fixed[i1][j1]) break;
    }
    int i2 = 0, j2 = 0;
    while (1) { // Choose second cell
        i2 = 3 * rowBox + randomInt(0, 2);
        j2 = 3 * colBox + randomInt(0, 2);
        if (!fixed[i2][j2]) break;
    }

    swap(grid[i1][j1], grid[i2][j2]); // Swap chosen cells
}

double stddev(vector<int>& costs) {
    int size = costs.size();
    double mean = 0;
    for (int i : costs) mean += i;
    mean = mean / size;
    double var = 0;
    for (int i : costs) var += (i - mean) * (i - mean);
    return var / size;
}

bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

vector<int> read_puzzle_from_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open puzzle file: " << filename << endl;
        exit(1);
    }
    
    string line;
    getline(file, line);
    file.close();

    vector<int> puzzle;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        puzzle.push_back(stoi(token));
    }
    return puzzle;
}

void write_csv_row(ofstream& file, const string& col1, const string& col2, const string& col3, const string& col4) {
    file << "| " << setw(10) << left << col1 << " | "
         << setw(20) << left << col2 << " | "
         << setw(20) << left << col3 << " | "
         << setw(25) << left << col4 << " |" << endl;
}

int main() {
    vector<vector<int>> grid(9, vector<int>(9)); // Stores grid
    vector<vector<bool>> fixed(9, vector<bool>(9)); // Stores fixed places

    // Read puzzle from file
    string puzzle_file = "EASY.txt";
    vector<int> puzzle = read_puzzle_from_file(puzzle_file);

    // Populate grid and fixed cells from puzzle
    int pos = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = puzzle[pos];
            if (grid[i][j]) fixed[i][j] = 1;
            pos++;
        }
    }

    vector<vector<int>> inputGrid = grid;
    cout << "\nInput Grid : \n";
    printGrid(inputGrid);

    // Start measuring performance
    auto start_time = chrono::high_resolution_clock::now();
    FILETIME creationTime, exitTime, kernelTime, userTime;
    ULARGE_INTEGER startUserTime, endUserTime;
    GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
    startUserTime.LowPart = userTime.dwLowDateTime;
    startUserTime.HighPart = userTime.dwHighDateTime;

    populateGrid(grid, fixed);

    vector<vector<int>> tempGrid; // Used to convey possible changes to main grid

    vector<int> costs;
    tempGrid = grid;
    for (int i = 0; i < 20; i++) {
        swapUnfixed(tempGrid, fixed);
        costs.push_back(getCost(tempGrid)); // Small number of neighbourhood moves to calculate appropriate initial temperature
    }

    // Set initial parameters
    int initialCost = getCost(grid);
    double initialTemp = stddev(costs);
    vector<vector<int>> initialGrid = grid;
    double alpha = 0.99;

    int currCost = initialCost;
    double currTemp = initialTemp;

    vector<vector<int>> currGrid = grid;
    int reheatPeriod = 200; // Sets reheat period
    queue<int> checkReheat; // Checks if cost has increased within reheat period
    int iterations = 0;
    int maxIterations = 40000;

    while (1) {
        if (iterations > maxIterations) break; // Found a solution (cost = 0)

        if (!currCost) {
            cout << "Perfect Solution!\n\n";
            break;
        }

        if (iterations < reheatPeriod)
            checkReheat.push(currCost);
        else {
            if (currCost > checkReheat.front()) { // Changed for the worse (probably) in past reheatPeriod
                currCost = initialCost; // Reset
                currTemp = initialTemp;
                currGrid = inputGrid;
                populateGrid(currGrid, fixed); // Re-populate initial grid

                cout << "Reheated to get new Populated Grid: \n";
                printGrid(currGrid);

                checkReheat = queue<int>();
                iterations = 0;
            } else {
                checkReheat.pop(); // Maintain most recent costs with queue
                checkReheat.push(currCost);
            }
        }

        tempGrid = currGrid;
        swapUnfixed(tempGrid, fixed);

        int tempCost = getCost(tempGrid);
        double delta = tempCost - currCost;

        if ((delta < 0) || (exp(-delta / currTemp) > randomDouble())) {
            currGrid = tempGrid;
            currCost = tempCost;
        }

        currTemp *= alpha;
        iterations++;
    }

    auto end_time = chrono::high_resolution_clock::now();
    //FILETIME exitTime, kernelTime, userTime;
   // ULARGE_INTEGER endUserTime;
    GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
    endUserTime.LowPart = userTime.dwLowDateTime;
    endUserTime.HighPart = userTime.dwHighDateTime;

    // Calculate elapsed time, CPU time, and memory usage
    chrono::duration<double> elapsed = end_time - start_time;
    double elapsed_seconds = elapsed.count();

    ULONGLONG cpu_time = (endUserTime.QuadPart - startUserTime.QuadPart) / 10000000.0;
    PROCESS_MEMORY_COUNTERS memCounter;
    GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
    size_t memory_usage = memCounter.WorkingSetSize / 1024; // Memory usage in KB

    // Open the log file and write performance metrics
    ofstream log_file("performance_log.csv");
    log_file << "Elapsed Time (seconds),CPU Time (seconds),Memory Usage (KB)\n";
    log_file //<< fixed << setprecision(3)
             << elapsed_seconds << ","
             << cpu_time << ","
             << memory_usage << "\n";
    log_file.close();

    cout << "Elapsed Time: " << elapsed_seconds << " seconds\n";
    cout << "CPU Time: " << cpu_time << " seconds\n";
    cout << "Memory Usage: " << memory_usage << " KB\n";

    return 0;
}
