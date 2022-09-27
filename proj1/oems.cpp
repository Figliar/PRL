#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <mpi.h>
#include <bits/stdc++.h>

using namespace std;
typedef unsigned char BYTE;

#define ARRAY_SIZE 8
// Set to 1 to measure time
#define MEASURE 0

/**
 * Parses input sequence and return array of integers
 */
int* get_numbers(){
    string myText;
    static int array[ARRAY_SIZE];
    ifstream MyReadFile("numbers");
    getline (MyReadFile, myText);
    for (int i = 0; i < ARRAY_SIZE; i++){
        array[i] = int(myText[i]) ;
    }
    MyReadFile.close();
    return array;
}

/**
 * Returns smaller number of first and second params
 * @param first
 * @param second
 */
int get_min(int *first, int *second){
    if (int(*first) <= int(*second)) {
        return *first;
    }
    else
        return *second;
}

/**
 * Returns bigger number of first and second params
 * @param first
 * @param second
 */
int get_max(int *first, int *second){
    if (int(*first) >= int(*second))
        return *first;
    else
        return *second;
}

/**
 * Measure time and print result to std out, it make diff of start time and end time
 * Source: http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
 * @param timeStart
 * @param timeEnd
 */
void measureTime(timespec timeStart, timespec timeEnd)
{
    timespec timeTemp;
    if ((timeEnd.tv_nsec - timeStart.tv_nsec) < 0)
    {
        timeTemp.tv_sec = timeEnd.tv_sec - timeStart.tv_sec - 1;
        timeTemp.tv_nsec = 1000000000 + timeEnd.tv_nsec - timeStart.tv_nsec;
    } else
    {
        timeTemp.tv_sec = timeEnd.tv_sec - timeStart.tv_sec;
        timeTemp.tv_nsec = timeEnd.tv_nsec - timeStart.tv_nsec;
    }
    cout<< timeTemp.tv_sec << ":" << timeTemp.tv_nsec << endl;
}


int main(int argc, char** argv) {
    /* Initialize variables to measure time */
    timespec timeStart;
    timespec timeEnd;
    if(MEASURE)
    {
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeStart);
    }

    /* Parse input to sequence of positive integers (0-255) and save to the array */
    int* numbers_raw;
    numbers_raw = new int[ARRAY_SIZE];
    numbers_raw = get_numbers();
    int* numbers;
    numbers = new int[ARRAY_SIZE];
    for ( int i = 0; i < ARRAY_SIZE; i++) {
        int num = ((int) numbers_raw[i] < 0) ? 127 - (int) numbers_raw[i] : (int) numbers_raw[i];
        numbers[i] = num;
    }

    /* Initialize MPI */
    int rank, size;
    MPI_Init (&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    /* Initialize necessary variables */
    int* sorted;
    sorted = new int[ARRAY_SIZE];
    int* max;
    max = new int;
    int* min;
    min = new int;
    int *rec1;
    rec1 = new int;
    int *rec2;
    rec2 = new int;

    /* Switch between processes */
    switch(rank) {
        case 0:
            for ( int i = 0; i < ARRAY_SIZE; i++) {
                cout<<numbers[i]<<(i < ARRAY_SIZE - 1 ? ", " : "\n");
            }
            *max = get_max(&numbers[0], &numbers[1]);
            *min = get_min(&numbers[0], &numbers[1]);
            MPI_Send(max,1,MPI_INT,5,50, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,4,40, MPI_COMM_WORLD);

            /* Put result to array in ascending order */
            MPI_Recv(&sorted[0], 1, MPI_INT, 10, 010, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[1], 1, MPI_INT, 16, 0161, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[2], 1, MPI_INT, 16, 0162, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[3], 1, MPI_INT, 17, 0171, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[4], 1, MPI_INT, 17, 0172, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[5], 1, MPI_INT, 18, 181, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[6], 1, MPI_INT, 18, 182, MPI_COMM_WORLD, &status);
            MPI_Recv(&sorted[7], 1, MPI_INT, 13, 013, MPI_COMM_WORLD, &status);

            /* Print measured time if MEASURE == 1 else print result of sort */
            if(MEASURE)
            {
                clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeEnd);
                measureTime(timeStart, timeEnd);
            }
            else
            {
                for ( int i = 0; i < ARRAY_SIZE; i++) {
                    cout << *(sorted+i) <<endl;
                }
            }
            break;
        case 1:
            *max = get_max(&numbers[2], &numbers[3]);
            *min = get_min(&numbers[2], &numbers[3]);
            MPI_Send(max,1,MPI_INT,5,51, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,4,41, MPI_COMM_WORLD);
            break;
        case 2:
            *max = get_max(&numbers[4], &numbers[5]);
            *min = get_min(&numbers[4], &numbers[5]);
            MPI_Send(max,1,MPI_INT,7,72, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,6,62, MPI_COMM_WORLD);
            break;
        case 3:
            *max = get_max(&numbers[6], &numbers[7]);
            *min = get_min(&numbers[6], &numbers[7]);
            MPI_Send(max,1,MPI_INT,7,73, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,6,63, MPI_COMM_WORLD);
            break;
        case 4:
            MPI_Recv(rec1, 1, MPI_INT, 0, 40, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 1, 41, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,8,84, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,10,104, MPI_COMM_WORLD);
            break;
        case 5:
            MPI_Recv(rec1, 1, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 1, 51, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,13,135, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,8,85, MPI_COMM_WORLD);
            break;
        case 6:
            MPI_Recv(rec1, 1, MPI_INT, 2, 62, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 3, 63, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,9,96, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,10,106, MPI_COMM_WORLD);
            break;
        case 7:
            MPI_Recv(rec1, 1, MPI_INT, 2, 72, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 3, 73, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,13,137, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,9,97, MPI_COMM_WORLD);
            break;
        case 8:
            MPI_Recv(rec1, 1, MPI_INT, 4, 84, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 5, 85, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,11,118, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,12,128, MPI_COMM_WORLD);
            break;
        case 9:
            MPI_Recv(rec1, 1, MPI_INT, 6, 96, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 7, 97, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,11,119, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,12,129, MPI_COMM_WORLD);
            break;
        case 10:
            MPI_Recv(rec1, 1, MPI_INT, 4, 104, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 6, 106, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,14,1410, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,0,010, MPI_COMM_WORLD);
            break;
        case 11:
            MPI_Recv(rec1, 1, MPI_INT, 8, 118, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 9, 119, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,18,1811, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,14,1411, MPI_COMM_WORLD);
            break;
        case 12:
            MPI_Recv(rec1, 1, MPI_INT, 8, 128, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 9, 129, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,15,1512, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,16,1612, MPI_COMM_WORLD);
            break;
        case 13:
            MPI_Recv(rec1, 1, MPI_INT, 5, 135, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 7, 137, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,0,013, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,15,1513, MPI_COMM_WORLD);
            break;
        case 14:
            MPI_Recv(rec1, 1, MPI_INT, 10, 1410, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 11, 1411, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,17,1714, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,16,1614, MPI_COMM_WORLD);
            break;
        case 15:
            MPI_Recv(rec1, 1, MPI_INT, 12, 1512, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 13, 1513, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(max,1,MPI_INT,18,1815, MPI_COMM_WORLD);
            MPI_Send(min,1,MPI_INT,17,1715, MPI_COMM_WORLD);
            break;
        case 16:
            MPI_Recv(rec1, 1, MPI_INT, 14, 1614, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 12, 1612, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(min,1,MPI_INT,0,0161, MPI_COMM_WORLD);
            MPI_Send(max,1,MPI_INT,0,0162, MPI_COMM_WORLD);
            break;
        case 17:
            MPI_Recv(rec1, 1, MPI_INT, 14, 1714, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 15, 1715, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(min,1,MPI_INT,0,0171, MPI_COMM_WORLD);
            MPI_Send(max,1,MPI_INT,0,0172, MPI_COMM_WORLD);
            break;
        case 18:
            MPI_Recv(rec1, 1, MPI_INT, 11, 1811, MPI_COMM_WORLD, &status);
            MPI_Recv(rec2, 1, MPI_INT, 15, 1815, MPI_COMM_WORLD, &status);
            *max = get_max(rec1, rec2);
            *min = get_min(rec1, rec2);
            MPI_Send(min,1,MPI_INT,0,181, MPI_COMM_WORLD);
            MPI_Send(max,1,MPI_INT,0,182, MPI_COMM_WORLD);
            break;
        default:
            break;
    }

    MPI_Finalize();

    return EXIT_SUCCESS;

}
