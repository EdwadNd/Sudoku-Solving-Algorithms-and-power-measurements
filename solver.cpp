#include<bits/stdc++.h>
#include <iostream>
#include <ctime>
using namespace std;

// get the grid
// randomly fill each 3x3 box such that it has all numbers from 1 to 9
// at each turn, keep swapping 2 random cells in a random box
// if cost increases within some iterations, randomly fill input grid again
// cost function would be sum of number of symbols missing all together
// from all rows and columns


// https://www.researchgate.net/publication/220403361_Metaheuristics_can_solve_Sudoku_puzzles

int SEED = 42;                                                // seed for PRNG
mt19937 rng(SEED);

int randomInt(int low, int high)
{
    uniform_int_distribution<> dist(low, high);
    return dist(rng);
}

double randomDouble()
{
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int getCost(vector<vector<int>>& grid)
{
    int cost = 0;
    for (int i=0; i<9; i++)
    {
        set<int> rowSet, columnSet;
        for (int j=0; j<9; j++)
        {
            rowSet.insert(grid[i][j]);                         // distinct elements in each row
            columnSet.insert(grid[j][i]);                      // distinct elements in each column
        }
        cost += (18 - (rowSet.size() + columnSet.size()));
    }
    return cost;
}

void printGrid(vector<vector<int>>& grid)
{
    for (int i=0; i<9; i++)
    {
        for (int j=0; j<9; j++)
        {
            cout<<grid[i][j]<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
}

void populateGrid(vector<vector<int>>& grid, vector<vector<bool>>& fixed)
{
    for (int rowBox=0; rowBox<3; rowBox++)
    {
        for (int colBox=0; colBox<3; colBox++)                  // go over each 3x3 box
        {

            set<int> boxSet;
            for (int i=1; i<10; i++) boxSet.insert(i);

            for (int i=3*rowBox; i<3*(rowBox+1); i++)
            {
                for (int j=3*colBox; j<3*(colBox+1); j++)       // maintain unfixed elements in each box
                {
                    if (fixed[i][j]) boxSet.erase(grid[i][j]);
                }
            }

            vector<int> boxUnfixed;
            for (int i:boxSet) boxUnfixed.push_back(i);
            shuffle(boxUnfixed.begin(), boxUnfixed.end(), rng); // shuffle unfixed elements

            int k = 0;
            for (int i=3*rowBox; i<3*(rowBox+1); i++)
            {
                for (int j=3*colBox; j<3*(colBox+1); j++)
                {
                    if (!fixed[i][j])
                    {
                        grid[i][j] = boxUnfixed[k];             // put in shuffled elements at empty places
                        k++;
                    }
                }
            }

        }
    }
}

void swapUnfixed(vector<vector<int>>& grid, vector<vector<bool>>& fixed)
{

    int rowBox = randomInt(0,2);
    int colBox = randomInt(0,2);
    int i1=0, j1=0;
    while (1)                                       // choose first cell
    {
        i1 = 3*rowBox + randomInt(0,2);
        j1 = 3*colBox + randomInt(0,2);
        if (!fixed[i1][j1]) break;
    }
    int i2=0, j2=0;
    while (1)                                       // choose second cell
    {
        i2 = 3*rowBox + randomInt(0,2);
        j2 = 3*colBox + randomInt(0,2);
        if (!fixed[i2][j2]) break;
    }

    swap(grid[i1][j1], grid[i2][j2]);               // swap chosen cells

}

double stddev(vector<int>& costs)
{
    int size = costs.size();
    double mean = 0;
    for (int i:costs) mean+=i;
    mean = mean/size;
    double var = 0;
    for (int i:costs) var += (i-mean)*(i-mean);
    return var/size;
}

int main()
{


    vector<vector<int>> grid(9, vector<int>(9));    //stores grid
    vector<vector<bool>>fixed(9,vector<bool>(9));   //stores fixed places


    /// 4 Easy Boards
    // string s = "079000046061042050050090020708001630000050000035600207020070060080230170490000380";   /// String  s represents the board so it is what you should read in from the file ///
    //string s = "910700000032609080007080900086030170300000006051020840009050300020301490000002061";
    //string s = "600705403003020700000006051205060904070000010901070508530800000008090300706100300";
    string s = "0,9,7,5,0,1,8,0,0,8,0,0,0,2,0,0,0,0,1,2,0,4,0,3,0,9,0,7,5,0,0,0,0,0,2,0,0,0,4,2,0,9,3,0,0,0,8,0,0,0,0,0,1,9,0,4,0,7,0,6,0,8,5,0,0,0,0,5,0,0,0,1,0,0,8,1,0,4,2,6,0";

    /// 4 Medium Boards
    //string s= "0,3,8,0,0,9,0,0,7,0,0,0,0,8,0,3,0,0,0,0,0,1,0,7,6,0,0,3,0,0,0,0,0,5,1,0,0,8,6,7,1,5,9,3,0,0,1,5,0,0,0,0,0,8,0,0,3,4,0,1,0,0,0,0,0,1,0,6,0,0,0,0,4,0,0,3,0,0,1,9,0";
    // string s= "080100507000047093300080160060203000800000006000408070038050004450870000906004080";
    // string s= "000159007006200000207060040000608091004020700170304000040010306000002500300986000";
    // string s ="800009500400007006050010470000900007740102035100003000029080060500600002004300009";

    /// 4 Hard Boards
    // string s= "0,0,8,0,0,2,0,6,0,0,0,0,1,0,0,5,0,3,0,0,1,0,5,0,4,7,0,0,2,4,5,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,9,4,0,0,5,3,0,8,0,6,0,0,4,0,7,0,0,9,0,0,0,0,6,0,3,0,0,8,0,0";
    //string s= "074800090003010800000090004000900302002000600601008000800020000005070100010003540";
    //string s= "800200009900060520000190300000070230000602000052010000009087000038040002400006001";
    // string s= "300209040400080100058410000130000200000000000004000035000098570001070003070105008";

    /// 4 Evil Boards:
    //string s= "0,0,0,0,0,0,0,0,4,0,5,0,0,0,3,0,9,6,0,0,9,6,0,0,0,7,0,0,0,0,0,4,9,7,0,0,0,0,3,8,0,5,2,0,0,0,0,7,2,6,0,0,0,0,0,6,0,0,0,0,7,5,0,2,9,0,4,0,0,0,3,0,3,0,0,0,0,0,0,0,0";
    //string s= "070300204800001009001200000700010008008000300200090005000008600100500002605004070";
    // string s= "008001070900000000000705420300000560007020900049000008016807000000000004090100800";
    // string s= "000927500000100009000000601060090300500010007004080020403000000100009000008462000";


    int pos = 0;
    for (int i=0; i<9; i++)
    {
        for (int j=0; j<9; j++)
        {
            if(s[pos]!=',')
            {
                grid[i][j] = s[pos]-'0';
                pos++;
            }
           else
            {
                pos++;
                grid[i][j] = s[pos]-'0';
                pos++;
            }


            if (grid[i][j]) fixed[i][j] = 1;
        }
    }

    vector<vector<int>> inputGrid = grid;
    cout<<"\nInput Grid : \n";
    printGrid(inputGrid);



    populateGrid(grid, fixed);

    /* cout<<"Populated Grid : \n";
     printGrid(grid);
     */

    vector<vector<int>> tempGrid;                   // used to convey possible changes to main grid

    vector<int> costs;
    tempGrid = grid;
    for (int i=0; i<20; i++)
    {
        swapUnfixed(tempGrid, fixed);
        costs.push_back(getCost(tempGrid));         // small number of neighbourhood moves
    }                                               // to calculate appropriate initial temperature


    // set initial parameters
    int initialCost = getCost(grid);
    double initialTemp = stddev(costs);
    vector<vector<int>> initialGrid = grid;
    double alpha = 0.99;

    int currCost = initialCost;
    double currTemp = initialTemp;

    vector<vector<int>> currGrid = grid;
    int reheatPeriod = 200;                         // sets reheat period
    queue<int> checkReheat;                         // checks if cost has increased within reheat period
    // for a need to reheat
    int iterations = 0;
    int maxIterations = 40000;

    while(1)
    {

        if (iterations > maxIterations) break;       // found a solution (cost = 0)

        if (!currCost)
        {
            cout<<"Perfect Solution!\n\n";
            break;
        }

        if (iterations<reheatPeriod)
            checkReheat.push(currCost);

        else
        {
            if (currCost > checkReheat.front())      // changed for the worse (probably) in past reheatPeriod
            {
                currCost = initialCost;              // reset
                currTemp = initialTemp;
                currGrid = inputGrid;
                populateGrid(currGrid, fixed);       // repopulate initial grid

                cout<<"Reheated to get new Populated Grid: \n";
                printGrid(currGrid);

                checkReheat = queue<int>();
                iterations = 0;

            }
            else
            {
                checkReheat.pop();                    // maintain most recent costs with queue
                checkReheat.push(currCost);
            }
        }

        tempGrid = currGrid;
        swapUnfixed(tempGrid, fixed);

        int tempCost = getCost(tempGrid);
        double delta = tempCost - currCost;

        if ( (delta<0) ||  (exp(-delta/currTemp) > randomDouble()) )
        {
            currGrid = tempGrid;
            currCost = tempCost;
        }

        currTemp *= alpha;
        iterations++;
    }

    cout<<"Final Grid :\n";
    printGrid(currGrid);


    long tick = clock();                                        // Clock function to return elapsed time in seconds
    cout << (long double)tick/CLOCKS_PER_SEC << endl <<endl;

    return 0;


}
