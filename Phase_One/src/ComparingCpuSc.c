
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "Merge.h"

#define MAX_METRIC 20
#define MIN_METRIC 5
#define MAX_PROCESSES 10
#define MIN_PROCESSES 5
/*
* TODO:
* 1) Come up with some constant data to test out functions. Done!
* 2) Start FCFS. Done
* 3) Test FCFS. Done
* 4) Start SJF. Done
* 5) Test SJF. Done
* 6) Randomize data. Done
*/

void calculate_Scheduling(struct Process * pArray, int size, int * totalWt, int * totalTt, int * totalIdlTime) // First-come first-serve function and shortest job first.
{
    // Have to include idle time when 
    int totalTime = 0;
    int idleTime = 0;
    int calcTurnaroundTime;
    int calcWaitTime;

    // Reconfigure, create a while if loop

    int i = 0;

    while(i < size)
    {
        if(totalTime >= pArray[i].arrivalTime)
        {   
            calcTurnaroundTime = (totalTime + pArray[i].burstTime) - pArray[i].arrivalTime;
            calcWaitTime = calcTurnaroundTime - pArray[i].burstTime;
            totalTime += pArray[i].burstTime;

            pArray[i].turnaroundTime = calcTurnaroundTime;
            pArray[i].waitTime = calcWaitTime;

            *totalWt += calcWaitTime;
            *totalTt += calcTurnaroundTime;

            i++;
        }else
        {
            if(i ==  0)
            {
                idleTime += pArray[i].arrivalTime;
                totalTime += idleTime;
            }else
            {
                idleTime += abs(totalTime - pArray[i].arrivalTime);
                totalTime += abs(totalTime - pArray[i].arrivalTime); 
            }

        }
    }

    printf("Total time to run all processes: %d\n", totalTime);
    printf("Total idle time: %d\n", idleTime);

    *totalIdlTime += idleTime;
}


void print_Processes(struct Process * pArray, int size)
{
    // Implement a readjustable table.
    printf("______________________________\n");
    printf("| Pi | A.T | B.T | W.T | T.T |\n");
    for(int i = 0; i < size; i++)
    {
        printf("| %s | %d | %d | %d | %d |\n", pArray[i].ProcessID, pArray[i].arrivalTime, pArray[i].burstTime, pArray[i].waitTime, pArray[i].turnaroundTime);
    }
    printf("______________________________\n");
}


void create_Processes(struct Process pArray[], int processes)
{

    int arrivalTime;
    int burstTime;

    for(int i = 0; i < processes; i++)
    {
        arrivalTime = rand() % (MAX_METRIC - MIN_METRIC + 1) + MIN_METRIC;
        burstTime = rand() % (MAX_METRIC - MIN_METRIC + 1) + MIN_METRIC;

        char processesID[10];
        snprintf(pArray[i].ProcessID, sizeof(pArray[i].ProcessID), "P%d", i); 

        pArray[i].arrivalTime = arrivalTime;
        pArray[i].burstTime = burstTime;
        pArray[i].turnaroundTime = 0;
        pArray[i].waitTime = 0;

    }

    //print_Processes(pArray, processes);
}


int main()
{
    // Create random data for both CPU scheduling algorithms and collect data.
    // testFCFS();
    // testSJF();
    
    clock_t start = clock();
    srand(time(NULL));

    int iterations = 100;
    int totalProcesses = 0;

    int totalWtFCFS = 0; // Total waittime for FCFS
    int totalTtFCFS = 0; // Total turnaround time for FCFS
    int totalIdlTimeFCFS = 0; // Total idle time for FCFS.

    int totalWTSJF = 0;
    int totalTtSJF = 0;
    int totalIdlTimeSJF = 0;

    for(int i = 0; i < iterations; i++)
    {
        printf("Iteration %d\n", i);

        int processes = rand() % (MAX_PROCESSES - MIN_PROCESSES + 1) + MIN_PROCESSES;
        totalProcesses += processes;
        struct Process pArray[processes];

        create_Processes(pArray, processes);

        mergeSort(pArray, 0, processes-1, 0); //FCFS
        calculate_Scheduling(pArray, processes, &totalWtFCFS, &totalTtFCFS, &totalIdlTimeFCFS);
        print_Processes(pArray, processes);


        mergeSort(pArray, 0, processes-1, 1); // SJF
        calculate_Scheduling(pArray, processes, &totalWTSJF, &totalTtSJF, &totalIdlTimeSJF);
        print_Processes(pArray, processes);

    }

    float avgWtFCFS = (float)totalWtFCFS / totalProcesses;
    float avgTtFCFS = (float)totalTtFCFS / totalProcesses;
    float avgIdlTimeFCFS = (float)totalIdlTimeFCFS / iterations;

    float avgWtSJF = (float)totalWTSJF / totalProcesses;
    float avgTtSJF = (float)totalTtSJF / totalProcesses;
    float avgIdlTimeSJF = (float)totalIdlTimeSJF / iterations;

    printf("\n");
    printf("Averages for %d iterations of FCFS for %d processes\n", iterations, totalProcesses);
    printf("Avg wait: %.2f\n", avgWtFCFS);
    printf("Avg T.T: %.2f\n", avgTtFCFS);
    printf("Avg idle: %.2f\n", avgIdlTimeFCFS);
    printf("\n");

    printf("\n");
    printf("Averages for %d iterations of SJF for %d processes\n", iterations, totalProcesses);
    printf("Avg wait: %.2f\n", avgWtSJF);
    printf("Avg T.T: %.2f\n", avgTtSJF);
    printf("Avg idle: %.2f\n", avgIdlTimeSJF);
    printf("\n");


    clock_t end = clock();

    printf("Run time: %.2fs\n", (float)(end - start)/CLOCKS_PER_SEC);

    return 1;
}