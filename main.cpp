#include <iostream>
#include "sudoku.h"
#include <string>
#include <chrono>
#include <iomanip>  // Include this for std::setprecision
#include <fstream>
#include <windows.h>
#include <psapi.h>


using namespace std; 

using namespace std::chrono;

// Use auto keyword to avoid typing long
// type definitions to get the timepoint
// at this instant use function now()


int main() {


    std::ofstream logFile("log.txt", std::ios_base::app);
    logFile << "Start Time: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n";
    auto start = high_resolution_clock::now();
    FILETIME creationTime, exitTime, kernelTime, userTime;
    GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
    ULARGE_INTEGER startUserTime;
    startUserTime.LowPart = userTime.dwLowDateTime;
    startUserTime.HighPart = userTime.dwHighDateTime;
    for(int i=0; i < 10; i++){    
     Sudoku sudoku;
     Difficulty chosenDifficulty = Difficulty::MEDIUM;
     sudoku.generate(chosenDifficulty);
  
     sudoku.draw();
    
     sudoku.solve_puzzle_BCktrc(0,0);
    /*  
    if (sudoku.solve_puzzle_BCktrc(0,0))
    {
        std::cout << "Time Elapsed: " << (float)(sudoku.timer2)/ CLOCKS_PER_SEC << " seconds.\n\n";
    }
    */
    
    sudoku.draw();
    //auto stop = high_resolution_clock::now();
    // auto duration = duration_cast<milliseconds>(stop - start);
 
    
    //sudoku.~Sudoku();
    }

     auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime);
    ULARGE_INTEGER endUserTime;
    endUserTime.LowPart = userTime.dwLowDateTime;
    endUserTime.HighPart = userTime.dwHighDateTime;
    
    logFile << " Elapsed Time: " << elapsed.count() << " seconds";
    logFile << " CPU Time: " << (endUserTime.QuadPart - startUserTime.QuadPart) / 1e7 << " seconds ";
    
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
    
    logFile << " Memory Usage: " << physMemUsedByMe / 1024 << " KB \n";
    logFile.close();






   std::cout << std::fixed << std::setprecision(5);
// To get the value of duration use the count()
// member function on the duration object

//cout << duration.count() << endl;
    cout << "End";
    string num;
    cin >> num;
    return 0;
}