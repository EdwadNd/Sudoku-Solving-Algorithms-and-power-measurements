#include "AbstractSudokuSolver.h"

AbstractSudokuSolver::AbstractSudokuSolver() : ConstraintProblem(), assignedCount(0) {
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			neighbours[i][j] = getNeighbours(PairIndex(i, j));
		}
	}
}

AbstractSudokuSolver::~AbstractSudokuSolver() {
}

const std::vector<PairIndex> AbstractSudokuSolver::getNeighbours(const PairIndex &idx) {
	std::vector<PairIndex> neighbours;

	for (int j = 0; j < GRID_HEIGHT; j++) {
		if (idx.second != j) {
			neighbours.push_back(PairIndex(idx.first, j));
		}
	}

	for (int i = 0; i < GRID_WIDTH; i++) {
		if (idx.first != i) {
			neighbours.push_back(PairIndex(i, idx.second));
		}
	}

	int outeri = idx.first / SQUARE_SIZE * SQUARE_SIZE;
	int outerj = idx.second / SQUARE_SIZE * SQUARE_SIZE;
	for (int i = 0; i < SQUARE_SIZE; i++) {
		for (int j = 0; j < SQUARE_SIZE; j++) {
			if (outeri + i != idx.first && outerj + j != idx.second)
				neighbours.push_back(PairIndex(outeri + i, outerj + j));
		}
	}
	return neighbours;
}

bool AbstractSudokuSolver::isAssignComplete() {
	return assignedCount == TOTAL_ENTRIES;
}

int AbstractSudokuSolver::getNeighbourMask(const PairIndex &idx) {
	int returnMask = 0;
	auto neighboring = neighbours[idx.first][idx.second];
	for (auto it = neighboring.begin(); it != neighboring.end(); ++it) {
		if (grid[it->first][it->second].isAssigned())
		{
			returnMask |= (0x1 << (grid[it->first][it->second].getValue() - 1));
		}
	}
	return returnMask;
}

bool AbstractSudokuSolver::checkConstraints(const PairIndex &idx, const int &value) {
	if (!(value > 0 && value <= MAX_VAL)) return false;
	int mask = 0x1 << (value - 1);
	return (getNeighbourMask(idx) & mask) == 0;
}

std::ostream &operator<<(std::ostream &out, const AbstractSudokuSolver &puzzle) {
	for (int i = 0; i < AbstractSudokuSolver::GRID_WIDTH; i++) {
		for (int j = 0; j < AbstractSudokuSolver::GRID_HEIGHT; j++) {
			out << puzzle.grid[i][j].getValue() << " ";
		}
		out << std::endl;
	}

	out << (puzzle.isSolved ? "Solved. " : "No Solution. "); // << "Visited " << puzzle.nodesVisited << " nodes in " << puzzle.elapsedTime << " microseconds.";
	return out;
}

int AbstractSudokuSolver::numberOfSetBits(int i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void AbstractSudokuSolver::shuffle(std::vector<int> &vect) {
	for (int i = vect.size() - 1; i >= 0; i--) {
		int swapidx = rand() % (i + 1);
		int temp = vect[swapidx];
		vect[swapidx] = vect[i];
		vect[i] = temp;
	}
}
