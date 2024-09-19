#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <windows.h>
#include <psapi.h>
#include <thread>
#include <array>
#include <sstream>
#include <sys/stat.h>

using namespace std;

#define MAX_K 1000
#define SIZE 9
struct Node {
	Node* left;
	Node* right;
	Node* up;
	Node* down;
	Node* head;
	int size;		// Used for Column header
	int rowID[3];	// Used to identify row in order to map solutions to a sudoku grid
};

const int SIZE_SQUARED = SIZE * SIZE;
const int SIZE_SQRT = sqrt((double)SIZE);
const int ROW_NB = SIZE * SIZE * SIZE;
const int COL_NB = 4 * SIZE * SIZE;

struct Node Head;
struct Node* HeadNode = &Head;
struct Node* solution[MAX_K];
struct Node* orig_values[MAX_K];
bool matrix[ROW_NB][COL_NB] = { { 0 } };
bool isSolved = false;




// ... [Keep all the existing struct and constant definitions] ...

void MapSolutionToGrid(int Sudoku[][SIZE]);
void PrintGrid(int Sudoku[][SIZE]);
void coverColumn(Node* col);
void uncoverColumn(Node* col);
void search(int k);  
void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]);
void BuildLinkedList(bool matrix[ROW_NB][COL_NB]);
void TransformListToCurrentGrid(int Puzzle[][SIZE]);
bool read_next_puzzle(ifstream& file, int Puzzle[SIZE][SIZE]);
bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
void write_csv_row(ofstream& file, const string& col1, const string& col2, const string& col3, const string& col4) {
    file << "| " << setw(10) << left << col1 << " | "
         << setw(20) << left << col2 << " | "
         << setw(20) << left << col3 << " | "
         << setw(25) << left << col4 << " |" << endl;
}

int main() {
    const string log_filename = "Hard_log.csv";
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

    ifstream puzzleFile("hard.txt");
    if (!puzzleFile.is_open()) {
        cout << "Unable to open file: " << endl;
        return 1;
    }

    int Puzzle[SIZE][SIZE];
    LARGE_INTEGER start_time, end_time, frequency;
    QueryPerformanceFrequency(&frequency);
    double total_elapsed_time = 0.0;
    double total_cpu_time = 0.0;
    SIZE_T peak_memory_usage = 0;
    PROCESS_MEMORY_COUNTERS_EX memInfo;
    FILETIME creationTime, exitTime, kernelTime, userTime;

    int count = 0;
    while (read_next_puzzle(puzzleFile, Puzzle)) {
        count++;
        cout << "\nSolving Puzzle #" << count << ":\n";
        PrintGrid(Puzzle);

        BuildSparseMatrix(matrix);
        BuildLinkedList(matrix);
        TransformListToCurrentGrid(Puzzle);

        ULARGE_INTEGER startUserTime, endUserTime;
        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        startUserTime.LowPart = userTime.dwLowDateTime;
        startUserTime.HighPart = userTime.dwHighDateTime;

        QueryPerformanceCounter(&start_time);
        isSolved = false;
        search(0);
        QueryPerformanceCounter(&end_time);

        double elapsed_time = static_cast<double>(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
        total_elapsed_time += elapsed_time;

        GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
        endUserTime.LowPart = userTime.dwLowDateTime;
        endUserTime.HighPart = userTime.dwHighDateTime;
        double cpu_time = (endUserTime.QuadPart - startUserTime.QuadPart) / 1e7;
        total_cpu_time += cpu_time;

        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo));
        SIZE_T peakMemoryUsed = memInfo.PeakWorkingSetSize;
        peak_memory_usage = max(peak_memory_usage, peakMemoryUsed);

        // Log performance data to CSV in a table-like format
        write_csv_row(logFile, 
                      to_string(count), 
                      to_string(elapsed_time), 
                      to_string(cpu_time), 
                      to_string(peakMemoryUsed / (1024.0 * 1024.0)));

        cout << "Puzzle " << count << " solved in " << elapsed_time << " seconds." << endl;
        cout << "CPU Time: " << cpu_time << " seconds" << endl;
        cout << "Peak Memory Usage: " << peakMemoryUsed / (1024 * 1024) << " MB" << endl;

        // Clear solution and orig_values for next puzzle
        for (int i = 0; i < MAX_K; i++) {
            solution[i] = nullptr;
            orig_values[i] = nullptr;
        }
    }

    // Print and log the total metrics for all puzzles
    cout << "\nTotal time taken for all puzzles: " << total_elapsed_time << " seconds." << endl;
    cout << "Total CPU time for all puzzles: " << total_cpu_time << " seconds" << endl;
    cout << "Peak memory usage across all puzzles: " << peak_memory_usage / (1024 * 1024) << " MB" << endl;

    logFile << "|------------|----------------------|----------------------|---------------------------|" << endl;
    write_csv_row(logFile, "Total", to_string(total_elapsed_time), to_string(total_cpu_time), to_string(peak_memory_usage / (1024.0 * 1024.0)));

    logFile.close();
    puzzleFile.close();

    return 0;
}


void coverColumn(Node* col) {
	col->left->right = col->right;
	col->right->left = col->left;
	for (Node* node = col->down; node != col; node = node->down) {
		for (Node* temp = node->right; temp != node; temp = temp->right) {
			temp->down->up = temp->up;
			temp->up->down = temp->down;
			temp->head->size--;
		}
	}
}

void uncoverColumn(Node* col) {
	for (Node* node = col->up; node != col; node = node->up) {
		for (Node* temp = node->left; temp != node; temp = temp->left) {
			temp->head->size++;
			temp->down->up = temp;
			temp->up->down = temp;
		}
	}
	col->left->right = col;
	col->right->left = col;
}

void search(int k) {
	if (HeadNode->right == HeadNode) {
        int Grid[SIZE][SIZE] = { {0} };
        MapSolutionToGrid(Grid);
        cout << "\nSolved Puzzle:\n";
        PrintGrid(Grid);
        isSolved = true;
        return;
    }

    Node* Col = HeadNode->right;
    for (Node* temp = Col->right; temp != HeadNode; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    coverColumn(Col);

    for (Node* temp = Col->down; temp != Col; temp = temp->down) {
        solution[k] = temp;
        for (Node* node = temp->right; node != temp; node = node->right) {
            coverColumn(node->head);
        }

        search(k + 1);

        temp = solution[k];
        solution[k] = NULL;
        Col = temp->head;
        for (Node* node = temp->left; node != temp; node = node->left) {
            uncoverColumn(node->head);
        }

        if (isSolved) return;
    }

    uncoverColumn(Col);
}

void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]) {
	int j = 0, counter = 0;
	for (int i = 0; i < ROW_NB; i++) {
		matrix[i][j] = 1;
		counter++;
		if (counter >= SIZE) {
			j++;
			counter = 0;
		}
	}

	int x = 0;
	counter = 1;
	for (j = SIZE_SQUARED; j < 2 * SIZE_SQUARED; j++) {
		for (int i = x; i < counter * SIZE_SQUARED; i += SIZE)
			matrix[i][j] = 1;

		if ((j + 1) % SIZE == 0) {
			x = counter * SIZE_SQUARED;
			counter++;
		}
		else
			x++;
	}

	j = 2 * SIZE_SQUARED;
	for (int i = 0; i < ROW_NB; i++) {
		matrix[i][j] = 1;
		j++;
		if (j >= 3 * SIZE_SQUARED)
			j = 2 * SIZE_SQUARED;
	}

	x = 0;
	for (j = 3 * SIZE_SQUARED; j < COL_NB; j++) {
		for (int l = 0; l < SIZE_SQRT; l++) {
			for (int k = 0; k < SIZE_SQRT; k++)
				matrix[x + l * SIZE + k * SIZE_SQUARED][j] = 1;
		}

		int temp = j + 1 - 3 * SIZE_SQUARED;

		if (temp % (int)(SIZE_SQRT * SIZE) == 0)
			x += (SIZE_SQRT - 1) * SIZE_SQUARED + (SIZE_SQRT - 1) * SIZE + 1;
		else if (temp % SIZE == 0)
			x += SIZE * (SIZE_SQRT - 1) + 1;
		else
			x++;
	}
}

void BuildLinkedList(bool matrix[ROW_NB][COL_NB]) {
	Node* header = new Node;
	header->left = header;
	header->right = header;
	header->down = header;
	header->up = header;
	header->size = -1;
	header->head = header;
	Node* temp = header;

	for (int i = 0; i < COL_NB; i++) {
		Node* newNode = new Node;
		newNode->size = 0;
		newNode->up = newNode;
		newNode->down = newNode;
		newNode->head = newNode;
		newNode->right = header;
		newNode->left = temp;
		temp->right = newNode;
		temp = newNode;
	}

	int ID[3] = { 0,1,1 };
	for (int i = 0; i < ROW_NB; i++) {
		Node* top = header->right;
		Node* prev = NULL;

		if (i != 0 && i % SIZE_SQUARED == 0) {
			ID[0] -= SIZE - 1;
			ID[1]++;
			ID[2] -= SIZE - 1;
		}
		else if (i != 0 && i % SIZE == 0) {
			ID[0] -= SIZE - 1;
			ID[2]++;
		}
		else {
			ID[0]++;
		}

		for (int j = 0; j < COL_NB; j++, top = top->right) {
			if (matrix[i][j]) {
				Node* newNode = new Node;
				newNode->rowID[0] = ID[0];
				newNode->rowID[1] = ID[1];
				newNode->rowID[2] = ID[2];
				if (prev == NULL) {
					prev = newNode;
					prev->right = newNode;
				}
				newNode->left = prev;
				newNode->right = prev->right;
				newNode->right->left = newNode;
				prev->right = newNode;
				newNode->head = top;
				newNode->down = top;
				newNode->up = top->up;
				top->up->down = newNode;
				top->size++;
				top->up = newNode;
				if (top->down == top)
					top->down = newNode;
				prev = newNode;
			}
		}
	}

	HeadNode = header;
}

void TransformListToCurrentGrid(int Puzzle[][SIZE]) {
	int index = 0;
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (Puzzle[i][j] > 0) {
				Node* Col = NULL;
				Node* temp = NULL;
				for (Col = HeadNode->right; Col != HeadNode; Col = Col->right) {
					for (temp = Col->down; temp != Col; temp = temp->down)
						if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
							goto ExitLoops;
				}
ExitLoops:		coverColumn(Col);
				orig_values[index] = temp;
				index++;
				for (Node* node = temp->right; node != temp; node = node->right) {
					coverColumn(node->head);
				}
			}
}

void MapSolutionToGrid(int Sudoku[][SIZE]) {
	for (int i = 0; solution[i] != NULL; i++) {
		Sudoku[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
	}
	for (int i = 0; orig_values[i] != NULL; i++) {
		Sudoku[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
	}
}

void PrintGrid(int Sudoku[][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++)
			cout << Sudoku[i][j] << " ";
		cout << endl;
	}
}

bool read_next_puzzle(ifstream& file, int Puzzle[SIZE][SIZE]) {
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string value;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (!getline(ss, value, ',')) {
                    cerr << "Error: Not enough values in the puzzle." << endl;
                    return false;
                }
                Puzzle[i][j] = stoi(value);
            }
        }
        return true;
    }
    return false;
}