#include <utility>
#include "AbstractSudokuSolver.h"
#include "ConstraintProblem.h"
#include "SudokuSolver_Basic.h"
#include "SudokuSolver_ForwardChecking.h"
#include "SudokuSolver_FC_Heuristics.h"
#include <ctime>

using namespace std;

map<PairIndex, int> list2map(const vector<int> &v) {
	map<PairIndex, int> sudokuMap;
	int count = 0;
	for (int i : v) {
		sudokuMap[PairIndex(count / 9, count % 9)] = i;
		count++;
	}
	return sudokuMap;
}

int main() {
	map<PairIndex, int> initialState;

	vector<int> trivial, simple, easy, medium, hard, evil;
	 /// 4 Easy Boards
    //easy = {0,7,9,0,0,0,0,4,6,0,6,1,0,4,2,0,5,0,0,5,0,0,9,0,0,2,0,7,0,8,0,0,1,6,3,0,0,0,0,0,5,0,0,0,0,0,3,5,6,0,0,2,0,7,0,2,0,0,7,0,0,6,0,0,8,0,2,3,0,1,7,0,4,9,0,0,0,0,3,8,0};
   //easy = {9,1,0,7,0,0,0,0,0,0,3,2,6,0,9,0,8,0,0,0,7,0,8,0,9,0,0,0,8,6,0,3,0,1,7,0,3,0,0,0,0,0,0,0,6,0,5,1,0,2,0,8,4,0,0,0,9,0,5,0,3,0,0,0,2,0,3,0,1,4,9,0,0,0,0,0,0,2,0,6,1};
    //  easy = {6,0,0,7,0,5,4,0,3,0,0,3,0,2,0,7,0,0,0,0,0,0,0,6,0,5,1,2,0,5,0,6,0,9,0,4,0,7,0,0,0,0,0,1,0,9,0,1,0,7,0,5,0,8,5,3,0,8,0,0,0,0,0,0,0,8,0,9,0,3,0,0,7,0,6,1,0,3,0,0,9};
      //  easy = {0,9,7,5,0,1,8,0,0,8,0,0,0,2,0,0,0,0,1,2,0,4,0,3,0,9,0,7,5,0,0,0,0,0,2,0,0,0,4,2,0,9,3,0,0,0,8,0,0,0,0,0,1,9,0,4,0,7,0,6,0,8,5,0,0,0,0,5,0,0,0,1,0,0,8,1,0,4,2,6,0};

   /// 4 Medium Boards
   //medium= {0,3,8,0,0,9,0,0,7,0,0,0,0,8,0,3,0,0,0,0,0,1,0,7,6,0,0,3,0,0,0,0,0,5,1,0,0,8,6,7,1,5,9,3,0,0,1,5,0,0,0,0,0,8,0,0,3,4,0,1,0,0,0,0,0,1,0,6,0,0,0,0,4,0,0,3,0,0,1,9,0};
    //medium= {0,8,0,1,0,0,5,0,7,0,0,0,0,4,7,0,9,3,3,0,0,0,8,0,1,6,0,0,6,0,2,0,3,0,0,0,8,0,0,0,0,0,0,0,6,0,0,0,4,0,8,0,7,0,0,3,8,0,5,0,0,0,4,4,5,0,8,7,0,0,0,0,9,0,6,0,0,4,0,8,0};
  // medium= {0,0,0,1,5,9,0,0,7,0,0,6,2,0,0,0,0,0,2,0,7,0,6,0,0,4,0,0,0,0,6,0,8,0,9,1,0,0,4,0,2,0,7,0,0,1,7,0,3,0,4,0,0,0,0,4,0,0,1,0,3,0,6,0,0,0,0,0,2,5,0,0,3,0,0,9,8,6,0,0,0};
  // medium ={8,0,0,0,0,9,5,0,0,4,0,0,0,0,7,0,0,6,0,5,0,0,1,0,4,7,0,0,0,0,9,0,0,0,0,7,7,4,0,1,0,2,0,3,5,1,0,0,0,0,3,0,0,0,0,2,9,0,8,0,0,6,0,5,0,0,6,0,0,0,0,2,0,0,4,3,0,0,0,0,9};

  /// 4 Hard Boards
   // hard= {0,0,8,0,0,2,0,6,0,0,0,0,1,0,0,5,0,3,0,0,1,0,5,0,4,7,0,0,2,4,5,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,9,4,0,0,5,3,0,8,0,6,0,0,4,0,7,0,0,9,0,0,0,0,6,0,3,0,0,8,0,0};
    //hard= {0,7,4,8,0,0,0,9,0,0,0,3,0,1,0,8,0,0,0,0,0,0,9,0,0,0,4,0,0,0,9,0,0,3,0,2,0,0,2,0,0,0,6,0,0,6,0,1,0,0,8,0,0,0,8,0,0,0,2,0,0,0,0,0,0,5,0,7,0,1,0,0,0,1,0,0,0,3,5,4,0};
    //hard= {8,0,0,2,0,0,0,0,9,9,0,0,0,6,0,5,2,0,0,0,0,1,9,0,3,0,0,0,0,0,0,7,0,2,3,0,0,0,0,6,0,2,0,0,0,0,5,2,0,1,0,0,0,0,0,0,9,0,8,7,0,0,0,0,3,8,0,4,0,0,0,2,4,0,0,0,0,6,0,0,1};
    //hard= {3,0,0,2,0,9,0,4,0,4,0,0,0,8,0,1,0,0,0,5,8,4,1,0,0,0,0,1,3,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,3,5,0,0,0,0,9,8,5,7,0,0,0,1,0,7,0,0,0,3,0,7,0,1,0,5,0,0,8};

   /// 4 Evil Boards:
     //   evil= {0,0,0,0,0,0,0,0,4,0,5,0,0,0,3,0,9,6,0,0,9,6,0,0,0,7,0,0,0,0,0,4,9,7,0,0,0,0,3,8,0,5,2,0,0,0,0,7,2,6,0,0,0,0,0,6,0,0,0,7,5,0,0,2,9,0,4,0,0,0,3,0,3,0,0,0,0,0,0,0,0};
    //  evil= {0,7,0,3,0,0,2,0,4,8,0,0,0,0,1,0,0,9,0,0,1,2,0,0,0,0,0,7,0,0,0,1,0,0,0,8,0,0,8,0,0,0,3,0,0,2,0,0,0,9,0,0,0,5,0,0,0,0,0,8,6,0,0,1,0,0,5,0,0,0,0,2,6,0,5,0,0,4,0,7,0};
    //evil= {0,0,8,0,0,1,0,7,0,9,0,0,0,0,0,0,0,0,0,0,0,7,0,5,4,2,0,3,0,0,0,0,0,5,6,0,0,0,7,0,2,0,9,0,0,0,4,9,0,0,0,0,0,8,0,1,6,8,0,7,0,0,0,0,0,0,0,0,0,0,0,4,0,9,0,1,0,0,8,0,0};
   evil= {0,0,0,9,2,7,5,0,0,0,0,0,1,0,0,0,0,9,0,0,0,0,0,0,6,0,1,0,6,0,0,9,0,3,0,0,5,0,0,0,1,0,0,0,7,0,0,4,0,8,0,0,2,0,4,0,3,0,0,0,0,0,0,1,0,0,0,0,9,0,0,0,0,0,8,4,6,2,0,0,0};




	initialState = list2map(evil);

	//ConstraintProblem<PairIndex, int> *solver = new SudokuSolver_ForwardChecking(initialState);
	ConstraintProblem<PairIndex, int> *solver = new SudokuSolver_FC_Heuristics(initialState);///Access Different Solvers For ForwARD cHECKING AND fORWARD cHECKING WITH hEURISTICS
	solver->backtrackingSearch();
	solver->printResult();


	delete solver;

  // clock to measure elapsed time
     long tick = clock();
	cout << endl << (double)tick/CLOCKS_PER_SEC;

}
