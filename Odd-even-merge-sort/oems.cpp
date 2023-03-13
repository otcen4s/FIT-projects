/**
 * file: oems.cpp
 * author: Matej Otčenáš (xotcen01)
 * date: 2022-04-08 
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>

#define TAG 0
#define INT MPI_INT

using namespace std;


// hardcoded matrix of processors where each processor has the ID for two receiving processors
// and for two sending processors
// in this case the processor ID = 0 is useless and but used for more simplistic indexing
                            // Rec_from1 | Rec_from2 | Send_to_min | Send_to_max |  ProcessID 
const unsigned RT [20][4] = {{    0,          0,           0,            0},           // 0
                             {    0,          0,           5,            6},           // 1
                             {    0,          0,           5,            6},           // 2
                             {    0,          0,           7,            8},           // 3
                             {    0,          0,           7,            8},           // 4
                             {    1,          2,           11,           9},           // 5
                             {    1,          2,           9,           14},           // 6
                             {    3,          4,          11,           10},           // 7
                             {    3,          4,          10,           14},           // 8
                             {    5,          6,          12,           13},           // 9
                             {    7,          8,          12,           13},           // 10
                             {    5,          7,           0,           15},           // 11
                             {    9,         10,          17,           16},           // 12
                             {    9,         10,          15,           19},           // 13
                             {    6,          8,          16,            0},           // 14
                             {    11,        13,          17,           18},           // 15
                             {    12,        14,          18,           19},           // 16
                             {    12,        15,           0,            0},           // 17
                             {    15,        16,           0,            0},           // 18
                             {    13,        16,           0,            0}};          // 19

 
// this is the hardcoded matrix for the master processor which sends the first 8 numbers received to the processor network
// and is also used for the final 8 numbers sorted receiving
                                // P1 | P2 | P3 | P4 | P5 | P6 | P7 | P8
const unsigned RT_MAIN [2][8] = {{ 1,   1,   2,   2,   3,   3,   4,   4  },  // Send_to
                                 { 11,  17,  17,  18,  18,  19,  19,  14 }}; // Rec_from



// read numbers from binary file 
std::vector<int> readNumbers()
{
    // load file
    std::ifstream file("numbers", std::ios::binary);

    // read data
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());

    file.close();
    
    vector<int> numbers;

    // convert data to int and store
    for (unsigned i = 0; i < data.size(); ++i) 
    {
        numbers.push_back(data[i]);
    }

    return numbers;
}


// main function
int main(int argc, char** argv) 
{
    // init MPI
    MPI_Init(NULL, NULL);

    int processID;

    // get id of new process
    MPI_Comm_rank(MPI_COMM_WORLD, &processID); 
   

    // master process for sending unsorted array and printing sorted one
    if (processID == 0)
    {   
        vector<int> numbers = readNumbers();
        
        // send 8 numbers to 8 processors
        for (unsigned i = 0; i < numbers.size(); i++)
        {
            cout << numbers[i] << " ";
            MPI_Send(&numbers[i], 1, INT, RT_MAIN[0][i], TAG, MPI_COMM_WORLD);
        }
        cout << endl;

        vector<int> sorted;
        int num;

        // receive 8 sorted numbers from 8 processors
        for (unsigned i = 0; i < numbers.size(); i++)
        {
            MPI_Recv(&num, 1, INT, RT_MAIN[1][i], TAG, MPI_COMM_WORLD, NULL);
            sorted.push_back(num);
        }

        // print to stdout
        for (unsigned i = 0; i < sorted.size(); i++)
        {
           cout << sorted[i] << endl;
        }
    }

    // other processes used as CE min-max comparator
    else
    {
        int in1, in2;

        // receive two input values from predefined senders
        MPI_Recv(&in1, 1, INT, RT[processID][0], 0, MPI_COMM_WORLD, NULL);
        MPI_Recv(&in2, 1, INT, RT[processID][1], 0, MPI_COMM_WORLD, NULL);
     
        int MIN = min(in1, in2);
        int MAX = max(in1, in2);
    
        // send min and max values to predefined receivers
        MPI_Send(&MIN, 1, INT, RT[processID][2], 0, MPI_COMM_WORLD); // send minimum value to other process
        MPI_Send(&MAX, 1, INT, RT[processID][3], 0, MPI_COMM_WORLD); // send maximum value to other process
    }
    
    // finalize MPI
    MPI_Finalize();

    return 0;
}