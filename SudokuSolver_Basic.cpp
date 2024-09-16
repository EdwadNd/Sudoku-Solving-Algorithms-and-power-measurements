#include "SudokuSolver_Basic.h"
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <random>

SudokuSolver_Basic::SudokuSolver_Basic(const std::map<PairIndex, int> &initialState) : AbstractSudokuSolver() {
	for (auto it = initialState.begin(); it != initialState.end(); ++it) {
		if (it->second > 0 && it->second <= MAX_VAL)
		{
			assignValue(it->first, it->second);
		}
	}
}

SudokuSolver_Basic::~SudokuSolver_Basic() {
}

const PairIndex SudokuSolver_Basic::selectNextVariable() {
	//RANDOM: get random unassigned variable
	int select = rand() % (TOTAL_ENTRIES - assignedCount);
	int curCount = 0;

	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			PairIndex curIdx = PairIndex(i, j);
			if (!grid[curIdx.first][curIdx.second].isAssigned()) {
				if (curCount == select)
				{
					return curIdx;
				}
				curCount++;
			}
		}
	}

	return PairIndex(-1, -1);
}

const std::vector<int> SudokuSolver_Basic::getValueOrder(const PairIndex &idx) {
	std::vector<int> validValues;
	for (int i = 1; i <= MAX_VAL; i++) {
		validValues.push_back(i);
	}
	//RANDOM: randomly selects value
	shuffle(validValues);
	return validValues;
}

void SudokuSolver_Basic::assignValue(const PairIndex &idx, const int &value) {
	grid[idx.first][idx.second].assignValue(value);
	assignedCount++;
}

void SudokuSolver_Basic::removeAssign(const PairIndex &idx) {
	grid[idx.first][idx.second].removeAssign();
	assignedCount--;
}
