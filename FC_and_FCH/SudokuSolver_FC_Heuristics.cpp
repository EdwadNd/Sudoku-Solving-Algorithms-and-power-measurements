#include "SudokuSolver_FC_Heuristics.h"
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <random>
#include <list>

SudokuSolver_FC_Heuristics::SudokuSolver_FC_Heuristics(const std::map<PairIndex, int> &initialState) : AbstractSudokuSolver() {
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			legalValues[i][j] = MAX_BIT_MASK;
			for (int k = 0; k < MAX_VAL; k++) {
				constraintsApplied[i][j][k] = 0;
			}
		}
	}

	for (auto it = initialState.begin(); it != initialState.end(); ++it) {
		if (it->second > 0 && it->second <= MAX_VAL)
		{
			assignValue(it->first, it->second);
		}
	}
}

SudokuSolver_FC_Heuristics::~SudokuSolver_FC_Heuristics() {
}

const PairIndex SudokuSolver_FC_Heuristics::selectNextVariable() {
	//H: most constrained variable heuristic
	int fewestLegals = INT_MAX;
	std::vector<PairIndex> mostConstrained;
	for (int i = 0; i < GRID_WIDTH; i++) {
		for (int j = 0; j < GRID_HEIGHT; j++) {
			PairIndex curIdx = PairIndex(i, j);
			if (!grid[curIdx.first][curIdx.second].isAssigned()) {
				int legals = numberOfSetBits(legalValues[curIdx.first][curIdx.second]);
				if (legals == fewestLegals) {
					mostConstrained.push_back(curIdx);
				} else if (legals < fewestLegals) {
					fewestLegals = legals;
					mostConstrained.clear();
					mostConstrained.push_back(curIdx);
				}
			}
		}
	}
	if (mostConstrained.size() == 1) return mostConstrained[0];
	else if (mostConstrained.size() == 0) return PairIndex(-1, -1);

	//H: most constraining variable heuristic to break ties.
	int mostConstraints = INT_MIN;
	std::vector<PairIndex> mostConstraining;
	for (auto it = mostConstrained.begin(); it != mostConstrained.end(); ++it) {
		int numConstraints = neighbours[it->first][it->second].size();
		if (numConstraints == mostConstraints) {
			mostConstraining.push_back(*it);
		} else {
			mostConstraints = numConstraints;
			mostConstraining.clear();
			mostConstraining.push_back(*it);
		}
	}

	if (mostConstraining.size() == 0) return PairIndex(-1, -1);
	else {
		//RANDOM: break tie randomly
		return mostConstraining[rand() % mostConstraining.size()];
	}
}

const std::vector<int> SudokuSolver_FC_Heuristics::getValueOrder(const PairIndex &idx) {
	//FC: get list of valid values
	std::vector<int> validValues;
	for (int i = 1; i <= MAX_VAL; i++) {
		if (legalValues[idx.first][idx.second] & (0x1 << (i - 1))) {
			validValues.push_back(i);
		}
	}
	//RANDOM: randomize order
	shuffle(validValues);

	//H: least constraining value heuristic
	int seen = 0;
	std::list<PairIndex> minPriorityList;
	for (auto validValueIt = validValues.begin(); validValueIt != validValues.end(); ++validValueIt) {
		seen++;
		int i = *validValueIt;
		int ruledOut = 0;

		auto neighboring = neighbours[idx.first][idx.second];
		for (auto it = neighboring.begin(); it != neighboring.end(); ++it) {
			if (!grid[it->first][it->second].isAssigned() && (legalValues[it->first][it->second] & 0x1 << (i - 1))) {
				ruledOut++;
			}
		}

		int priority = ruledOut * 10 + seen;

		auto listIt = minPriorityList.begin();
		for (; listIt != minPriorityList.end(); ++listIt) {
			if (listIt->first > priority) break;
		}
		minPriorityList.insert(listIt, PairIndex(priority, i));
	}

	std::vector<int> returnOrder;
	for (auto listIt = minPriorityList.begin(); listIt != minPriorityList.end(); ++listIt) {
		returnOrder.push_back(listIt->second);
	}
	return returnOrder;
}

bool SudokuSolver_FC_Heuristics::checkConstraints(const PairIndex &idx, const int &value) {
	bool checkBase = AbstractSudokuSolver::checkConstraints(idx, value);
	if (checkBase == false) return false;

	//FC: check the forward checking condition - check the variables that potentially are updated only.
	auto neighboring = neighbours[idx.first][idx.second];
	for (auto it = neighboring.begin(); it != neighboring.end(); ++it) {
		if (!grid[it->first][it->second].isAssigned() && legalValues[it->first][it->second] == (0x1 << (value - 1))) {
			return false;
		}
	}
	return true;
}

void SudokuSolver_FC_Heuristics::assignValue(const PairIndex &idx, const int &value) {
	grid[idx.first][idx.second].assignValue(value);
	assignedCount++;
	
	//FC: update legal values states
	auto neighboring = neighbours[idx.first][idx.second];
	for (auto it = neighboring.begin(); it != neighboring.end(); ++it) {
		if (++constraintsApplied[it->first][it->second][value - 1] == 1) {
			legalValues[it->first][it->second] &= ~(0x1 << (value - 1));
		}
	}
}

void SudokuSolver_FC_Heuristics::removeAssign(const PairIndex &idx) {
	int value = grid[idx.first][idx.second].getValue();
	grid[idx.first][idx.second].removeAssign();
	assignedCount--;

	//FC: update legal values states
	auto neighboring = neighbours[idx.first][idx.second];
	for (auto it = neighboring.begin(); it != neighboring.end(); ++it) {
		if (--constraintsApplied[it->first][it->second][value - 1] == 0) {
			legalValues[it->first][it->second] |= (0x1 << (value - 1));
		}
	}
}

