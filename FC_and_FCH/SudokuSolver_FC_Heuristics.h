#pragma once
#include "AbstractSudokuSolver.h"
#include "ConstraintProblem.h"
#include <utility>

class SudokuSolver_FC_Heuristics : virtual public AbstractSudokuSolver {
public:
	//constructor
	SudokuSolver_FC_Heuristics(const std::map<PairIndex, int> &initialState);
	//destructor
	virtual ~SudokuSolver_FC_Heuristics();
protected:
	//bitmask of legal values.
	std::array<std::array<int, GRID_HEIGHT>, GRID_WIDTH> legalValues;
	//array of number of times the value was constrained
	std::array<std::array<std::array<int, MAX_VAL>, GRID_HEIGHT>, GRID_WIDTH> constraintsApplied;

	//RANDOM + H: selects the variable with the lowest number of remaining valid values left
	//breaking ties by the most constraining, then randomly
	virtual const PairIndex selectNextVariable();
	//RANDOM + FC + H: selects the most constraining valid value in the variable at idx (FC constraints), break ties randomly
	virtual const std::vector<int> getValueOrder(const PairIndex &idx);
	//FC: checks the forward checking condition as well as normal validity constraints.
	virtual bool checkConstraints(const PairIndex &idx, const int &value);

	//FC: assigns the value to variable at idx.
	virtual void assignValue(const PairIndex &idx, const int &value);
	//FC: removes the assignment at idx.
	virtual void removeAssign(const PairIndex &idx);
};