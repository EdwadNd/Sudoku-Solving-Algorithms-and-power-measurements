#pragma once
#include "AbstractSudokuSolver.h"
#include "ConstraintProblem.h"
#include <utility>

class SudokuSolver_Basic : virtual public AbstractSudokuSolver {
public:
	//constructor
	SudokuSolver_Basic(const std::map<PairIndex, int> &initialState);
	//destructor
	virtual ~SudokuSolver_Basic();
protected:
	//RANDOM: selects a random unassigned variable
	virtual const PairIndex selectNextVariable();
	//RANDOM: selects a random value from 1-9 in the current variable
	virtual const std::vector<int> getValueOrder(const PairIndex &idx);
	//assigns the value to variable at idx.
	virtual void assignValue(const PairIndex &idx, const int &value);
	//removes the assignment at idx.
	virtual void removeAssign(const PairIndex &idx);
};