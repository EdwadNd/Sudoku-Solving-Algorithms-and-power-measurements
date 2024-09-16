#pragma once
#include "AbstractSudokuSolver.h"
#include "ConstraintProblem.h"
#include <utility>

class SudokuSolver_ForwardChecking : virtual public AbstractSudokuSolver {
public:
	//constructor
	SudokuSolver_ForwardChecking(const std::map<PairIndex, int> &initialState);
	//destructor
	virtual ~SudokuSolver_ForwardChecking();
protected:
	//bitmask of legal values.
	std::array<std::array<int, GRID_HEIGHT>, GRID_WIDTH> legalValues;
	//array of number of times the value was constrained
	std::array<std::array<std::array<int, MAX_VAL>, GRID_HEIGHT>, GRID_WIDTH> constraintsApplied;

	//RANDOM: selects a random unassigned variable
	virtual const PairIndex selectNextVariable();
	//RANDOM + FC: selects a random valid value in the variable at idx (FC constraints)
	virtual const std::vector<int> getValueOrder(const PairIndex &idx);
	//FC: checks the forward checking condition as well as normal validity constraints.
	virtual bool checkConstraints(const PairIndex &idx, const int &value);

	//FC: assigns the value to variable at idx.
	virtual void assignValue(const PairIndex &idx, const int &value);
	//FC: removes the assignment at idx.
	virtual void removeAssign(const PairIndex &idx);
};