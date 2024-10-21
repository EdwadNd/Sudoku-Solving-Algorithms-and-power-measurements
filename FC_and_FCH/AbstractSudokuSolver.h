#pragma once
#include "ConstraintProblem.h"
#include <utility>
#include <array>
#include <map>
#include <iostream>

typedef std::pair<int, int> PairIndex;

//base class for the three sudoku solvers. Implements ConstraintProblem.
class AbstractSudokuSolver : virtual public ConstraintProblem<PairIndex, int> {
public:
	//constructor
	AbstractSudokuSolver();
	//destructor
	virtual ~AbstractSudokuSolver();
	//print to std::cout
	virtual void printResult() {
		std::cout << *this << std::endl;
	}
	//stream operator overload
	friend std::ostream &operator<<(std::ostream &out, const AbstractSudokuSolver &puzzle);

protected:
	//constants
	static const int GRID_WIDTH = 9, GRID_HEIGHT = 9, MAX_VAL = 9;
	static const int SQUARE_SIZE = 3;
	static const int MAX_BIT_MASK = 0x000001ff;
	static const int TOTAL_ENTRIES = 81;

	//the grid
	std::array<std::array<Variable, GRID_HEIGHT>, GRID_WIDTH> grid;
	//the neighbours of each square in the grid.
	std::array<std::array<std::vector<PairIndex>, GRID_HEIGHT>, GRID_WIDTH> neighbours;

	//number of squares currently assigned
	int assignedCount;

	//checks the sudoku constraints with respect to the board and a potential new value &value.
	virtual bool checkConstraints(const PairIndex &idx, const int &value);
	//checks if all 81 squares are filled.
	virtual bool isAssignComplete();

	//implement in subclasses - here is where forward checking, randomizing and heuristics are implemented.
	virtual void assignValue(const PairIndex &idx, const int &value) = 0;
	virtual void removeAssign(const PairIndex &idx) = 0;
	virtual const PairIndex selectNextVariable() = 0;
	virtual const std::vector<int> getValueOrder(const PairIndex &idx) = 0;


	//gets the mask of the neighboring numbers.
	int getNeighbourMask(const PairIndex &idx);
	//calculates hamming weight
	int numberOfSetBits(int i);
	//suffles an array
	void shuffle(std::vector<int> &vect);
	//retrieves the complete set of neighboring squares of a square at idx.
	const std::vector<PairIndex> getNeighbours(const PairIndex &idx);
};
