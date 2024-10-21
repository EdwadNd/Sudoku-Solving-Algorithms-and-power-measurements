#pragma once
#include <vector>
#include <chrono>
#include <windows.h>

template<class VarIndex, class VarType>
class ConstraintProblem {
public:
	//constructor
	ConstraintProblem() : nodesVisited(0), elapsedTime(0), isSolved(false) {
		srand(getTimeInMicroseconds());
	}

	//destructor
	virtual ~ConstraintProblem() {
	}

	//main backtracking algorithm wrapper
	virtual bool backtrackingSearch() {
		unsigned long startTime = getTimeInMicroseconds();
		bool result = backtrackingSearch_Recursive();
		elapsedTime = getTimeInMicroseconds() - startTime;
		isSolved = result;
		return result;
	};

	//print to std::cout
	virtual void printResult() = 0;

protected:
	//main backtracking algorithm
	virtual bool backtrackingSearch_Recursive() {
		nodesVisited++;
		if (isAssignComplete()) return true;

		VarIndex idx = selectNextVariable();
		std::vector<VarType> valueOrder = getValueOrder(idx);
		for (auto it = valueOrder.begin(); it != valueOrder.end(); ++it)
		{
			if (checkConstraints(idx, *it)) {
				assignValue(idx, *it);
				if (backtrackingSearch_Recursive()) return true;
				else removeAssign(idx);
			}
		}
		return false;
	};

	//Variable class to represent possible variants
	class Variable {
		VarType value;
		bool assigned;

	public:
		//constructor
		Variable() : assigned(false) {
			value = VarType();
		}

		//assigns the value val to the variable.
		void assignValue(const VarType &val) {
			value = val;
			assigned = true;
		}

		//removes the assigned state from the variable.
		void removeAssign() {
			value = VarType();
			assigned = false;
		}

		//checks if variable is assigned.
		bool isAssigned() {
			return assigned;
		}

		//retrieves the value of the variable's current assignment.
		VarType getValue() const {
			return value;
		}
	};

	//statistics and state
	unsigned long nodesVisited;
	unsigned long elapsedTime;
	bool isSolved;

	//checks if assignment is complete.
	virtual bool isAssignComplete() = 0;

	//checks constraints
	virtual bool checkConstraints(const VarIndex &selectedVar, const VarType &value) = 0;

	//assign and de-assign variable
	virtual void assignValue(const VarIndex &selectedVar, const VarType &value) = 0;
	virtual void removeAssign(const VarIndex &selectedVar) = 0;

	//retrieve the next variable to look at.
	virtual const VarIndex selectNextVariable() = 0;

	//retrieve the order to try variable values.
	virtual const std::vector<VarType> getValueOrder(const VarIndex &idx) = 0;

	//PLATFORM SPECIFIC - windows timer fn
	unsigned long getTimeInMicroseconds()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		double PCFreq = double(li.QuadPart) / 1000000.0;

		QueryPerformanceCounter(&li);
		return (unsigned long)(li.QuadPart / PCFreq);
	}
};

