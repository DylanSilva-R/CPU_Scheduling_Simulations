
/*
 * TODO:
 * 1) Implement deadlock conditions
 *   1.1) Mutual exclusion
 *   1.2) Hold and wait
 *   1.3) No preemption
 *   1.4) Circular wait
 * 2) Implement deadlock detection: Banker's algorithm
 * 3) Simulate deadlock by implementing deadlock prone code.
 * 4) Deadlock handling: Banker's algorithm
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "Merge.h"

#define RESOURCE_SIZE 5
#define BILLION 1000000000L;

pthread_mutex_t lock;

struct Process_With_R // Struct to define processes with resources
{
    /*
    * Will need to readjust this struct. Need to implement dynamic memory allocation.
    */
    char processID[50];
    int resourceAllocation[RESOURCE_SIZE];
    int resourceRequestMax[RESOURCE_SIZE];
    int resourceAvailable[RESOURCE_SIZE];
    int resourceNeed[RESOURCE_SIZE];
    int processRan; // Only 0 or 1 symbolizing true or false.
    int safeSequenceIndex[RESOURCE_SIZE];
    int runTime;
    // int SizeOfPArray;
    // int resourceCount;
    // int waitTime;
    // int turnAroundTime;
};

void *process_Action(void *arg) // Process performing an action. Process threads will use this function.
{
    /*
     * - This function has mutex implemented to unlock and lock resources.
     * -
     */

    struct Process_With_R *process = (struct Process_With_R *)arg;

    pthread_mutex_lock(&lock);

    printf("Process %s is running.\n", process->processID);
    printf("Process run time is %d\n", process->runTime);
    sleep(process->runTime);

    pthread_mutex_unlock(&lock);
    free(process);
    pthread_exit(NULL);
}

void print_Bankers_Algo(struct Process_With_R *pArray, int sizeOfPArray, int resourceCount)
{
    for (int i = 0; i < sizeOfPArray; i++)
    {
        printf("%s: ", pArray[i].processID);

        printf("|");
        for (int j = 0; j < resourceCount; j++)
        {
            printf("%d ", pArray[i].resourceAllocation[j]);
        }
        printf("|");

        for (int j = 0; j < resourceCount; j++)
        {
            printf("%d ", pArray[i].resourceRequestMax[j]);
        }
        printf("|");

        for (int j = 0; j < resourceCount; j++)
        {
            printf("%d ", pArray[i].resourceAvailable[j]);
        }
        printf("|");

        for (int j = 0; j < resourceCount; j++)
        {
            printf("%d ", pArray[i].resourceNeed[j]);
        }
        printf("|");

        printf("\n");
    }

    printf("Safe sequence: ");
    for (int i = 0; i < sizeOfPArray - 1; i++)
    {

        if (i < sizeOfPArray - 2)
        {
            printf("%s => ", pArray[pArray[0].safeSequenceIndex[i]].processID);
        }
        else
        {
            printf("%s", pArray[pArray[0].safeSequenceIndex[i]].processID);
        }
    }
    printf("\n");
}

int bankers_Algo_Avoidance(struct Process_With_R *pArray, int sizeOfPArray, int resourceCount)
{
    /*
     * Purpose of function:
     *   - Calculate the safe sequence using Bankers Algorithm
     *   - Return safe sequence error code. 1 safe sequence was sucessfully created, 0 safe sequence was not created.
     */

    int resourceAllocationTotals[resourceCount];
    int total;
    int calcNeed;
    int successfully_Calculated = 0;

    // First, calculate total for all process allocations.

    for (int i = 0; i < resourceCount; i++)
    {
        total = 0;

        for (int j = 0; j < sizeOfPArray; j++)
        {
            total += pArray[j].resourceAllocation[i];
        }

        resourceAllocationTotals[i] = total;
    }

    // Second, calculate need matrix.

    for (int i = 0; i < sizeOfPArray; i++)
    {
        for (int j = 0; j < resourceCount; j++)
        {
            calcNeed = pArray[i].resourceRequestMax[j] - pArray[i].resourceAllocation[j];
            pArray[i].resourceNeed[j] = calcNeed;
        }
    }

    /*
     * Third, find the safe sequence
     */

    for (int i = 0; i < sizeOfPArray; i++)
    {
        for (int j = 0; j < sizeOfPArray; j++)
        {
            int countValidRes = 0;
            int holdAllocationRes;
            int holdAvailableRes;
            int holdNeedRes;

            if (pArray[j].processRan == 0)
            {
                for (int k = 0; k < resourceCount; k++)
                {
                    holdAvailableRes = pArray[i].resourceAvailable[k];
                    holdNeedRes = pArray[j].resourceNeed[k];

                    if (holdNeedRes <= holdAvailableRes)
                    {
                        countValidRes++;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (countValidRes == resourceCount)
            {

                if (i < sizeOfPArray)
                {
                    pArray[0].safeSequenceIndex[i] = j;
                    // safeSequenceIndex[i] = j;
                    pArray[j].processRan = 1;

                    for (int k = 0; k < resourceCount; k++)
                    {
                        holdAllocationRes = pArray[j].resourceAllocation[k];
                        holdAvailableRes = pArray[i].resourceAvailable[k];

                        pArray[i + 1].resourceAvailable[k] = holdAllocationRes + holdAvailableRes;
                    }
                }
                successfully_Calculated++;
                break;
            }
        }
    }

    if (successfully_Calculated == sizeOfPArray)
    {
        return 1; // Safe sequence exists
    }
    else
    {
        return 0; // There is no safe sequence
    }
}

void run_Threads(struct Process_With_R *pArray, int sizeOfPArray)
{
    // Last element is there to calculate the last available matrix value.

    int safeSequenceIndex[sizeOfPArray];

    pthread_t processes[sizeOfPArray];
    pthread_mutex_init(&lock, NULL);

    // Create threads.

    for (int i = 0; i < sizeOfPArray; i++)
    {
        struct Process_With_R *hold = (struct Process_With_R *)malloc(sizeof(struct Process_With_R));

        if (hold == NULL)
        {
            perror("Memory allocation failed.\n");
        }

        *hold = pArray[pArray->safeSequenceIndex[i]];

        if (pthread_create(processes + i, NULL, &process_Action, (void *)hold) != 0)
        {
            printf("Error creating thread.\n");
            free(hold);
        }

        printf("Process %d started.\n", i);
    }

    // Join threads.

    for (int i = 0; i < sizeOfPArray; i++)
    {
        if (pthread_join(processes[i], NULL) != 0)
        {
            printf("Error joining threads.\n");
        }

        printf("Process %d finished runnning.\n", i);
    }

    // Destroy mutex lock.
    pthread_mutex_destroy(&lock);
}

int isInteger(char *input)
{
    // Strip leading and trailing whitespaces
    while (*input == ' ' || *input == '\t')
    {
        input++;
    }

    // If input is empty after trimming, return 0 (false)
    if (*input == '\0')
    {
        return 0;
    }

    // Check for an optional sign ('+' or '-')
    if (*input == '+' || *input == '-')
    {
        input++;
    }

    // Check if all characters are digits
    while (*input)
    {
        if (!isdigit(*input))
        {
            return 0; // Not an integer if there's a non-digit character
        }
        input++;
    }

    // Input is a valid integer if we've only seen digits (and possibly a sign)
    return 1;
}

int validate_Integer_Input()
{
    /*
     * Take in user input and check if its an integer or not.
     */
    int true = 1;
    int num;

    while (true)
    {
        num = 0;

        char buffer[100];

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = '\0';

            if (isInteger(buffer))
            {
                num = atoi(buffer);
                true = 0;
            }
            else
            {
                printf("Invalid input. Please try again.\n");
            }
        }
        else
        {
            printf("Error reading input.\n");
            continue;
        }
    }

    return num;
}

void menu()
{
    printf("1) Low processes\n");
    printf("2) Medium processes\n");
    printf("3) High processes\n");
    printf("4) EXIT\n");
}

int main()
{
    /*
     *   Main Function Structure
     *   _______________________
     *
     *   - Create processes array.
     *   - Create Safe sequence array
     *   - Calculate safe sequence using
     *
     *
     */

    printf(" _______________________\n");
    printf("|CPU Deadlock Simulation|\n");
    printf("|_______________________|\n");
    printf("By: Dylan Silva-Rivas\n");
    printf("\n");

    // Small amount of processes
    struct Process_With_R pArraySmall[] = {{"P0", {0, 0, 1, 2, 1}, {0, 0, 1, 2, 5}, {1, 5, 2, 0, 2}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}, // Process, allocation, max resource request, resource need
                                           {"P1", {1, 0, 0, 0, 3}, {1, 7, 5, 0, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P2", {1, 3, 5, 4, 2}, {2, 3, 5, 6, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P3", {0, 6, 3, 2, 5}, {0, 6, 5, 2, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P4", {0, 0, 1, 4, 6}, {0, 6, 5, 6, 10}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P5", {0, 1, 2, 9, 6}, {1, 2, 3, 10, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"Empty", {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}};
    // Last element is there to calculate the last available matrix value.

    // Medium amount of processes
    struct Process_With_R pArrayMed[] = {{"P0", {0, 0, 1, 2, 1}, {0, 0, 1, 2, 5}, {1, 5, 2, 0, 2}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}, // Process, allocation, max resource request, resource need
                                         {"P1", {1, 0, 0, 0, 3}, {1, 7, 5, 0, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P2", {1, 3, 5, 4, 2}, {2, 3, 5, 6, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P3", {0, 6, 3, 2, 5}, {0, 6, 5, 2, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P4", {0, 0, 1, 4, 6}, {0, 6, 5, 6, 10}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P5", {0, 2, 2, 9, 6}, {1, 3, 4, 9, 8}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P6", {0, 3, 3, 10, 7}, {1, 4, 3, 10, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P7", {0, 5, 4, 3, 8}, {2, 6, 6, 8, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P8", {0, 2, 3, 2, 3}, {5, 5, 5, 6, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P9", {0, 3, 5, 1, 2}, {6, 4, 6, 9, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"P10", {0, 2, 2, 9, 6}, {1, 3, 3, 11, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                         {"Empty", {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}};
    // Last element is there to calculate the last available matrix value.

    // Large amount of processes
    struct Process_With_R pArrayLarge[] = {{"P0", {0, 0, 1, 2, 1}, {0, 0, 1, 2, 5}, {1, 5, 2, 0, 2}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}, // Process, allocation, max resource request, resource need
                                           {"P1", {1, 0, 0, 0, 3}, {1, 7, 5, 0, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P2", {1, 3, 5, 4, 2}, {2, 3, 5, 6, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P3", {0, 6, 3, 2, 5}, {0, 6, 5, 2, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P4", {0, 0, 1, 4, 6}, {0, 6, 5, 6, 10}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           
                                           {"P5", {2, 4, 3, 8, 6}, {3, 5, 3, 9, 1}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P6", {5, 2, 4, 5, 7}, {6, 3, 5, 6, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P7", {5, 2, 2, 4, 8}, {11, 5, 6, 4, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P8", {6, 6, 2, 5, 9}, {7, 8, 3, 6, 10}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P9", {1, 5, 2, 8, 1}, {2, 6, 5, 9, 10}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P10", {2, 8, 2, 5, 2}, {5, 9, 4, 6, 5}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P11", {3, 9, 2, 2, 2}, {4, 10, 3, 4, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P12", {1, 10, 2, 1, 3}, {1, 11, 4, 4, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P13", {2, 3, 2, 6, 4}, {3, 5, 3, 8, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P14", {7, 2, 2, 7, 5}, {8, 3, 4, 8, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P15", {9, 1, 2, 8, 7}, {12, 2, 5, 9, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P16", {8, 2, 2, 2, 5}, {9, 2, 3, 4, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P17", {6, 5, 2, 5, 6}, {7, 9, 5, 5, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P18", {5, 7, 2, 9, 8}, {9, 7, 3, 10, 12}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P19", {3, 6, 2, 1, 7}, {12, 10, 15, 13, 9}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"P20", {1, 5, 2, 4, 2}, {20, 22, 24, 25, 29}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}},
                                           {"Empty", {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, 0, {0, 0, 0, 0, 0}}};

    int loop = 1;
    struct timespec res;
    long start;
    long end;
    double runTime;
    int sizeOfPArray;
    int resourceCount;
    int safeSequenceCode;

    while (loop)
    {
        menu();

        printf("Enter menu option: ");
        int menuInput = validate_Integer_Input();
        printf("\n");

        switch (menuInput)
        {
            case 1:
                sizeOfPArray = (sizeof(pArraySmall) / sizeof(pArraySmall[0]));             // Size of process array.
                resourceCount = (sizeof(pArraySmall[1].resourceAllocation) / sizeof(int)); // Size of resources.

                printf("Amount of processes: %d\n", sizeOfPArray-1);
                printf("Amount of resources: %d\n", resourceCount);

                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                start = res.tv_nsec;

                safeSequenceCode = bankers_Algo_Avoidance(pArraySmall, sizeOfPArray, resourceCount);
                
                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                end = res.tv_nsec;
                runTime = end - start;

                print_Bankers_Algo(pArraySmall, sizeOfPArray, resourceCount);
                printf("Banker's algorithm runtime: %lfns\n", runTime);

                run_Threads(pArraySmall, sizeOfPArray-1);
            
                break;
            case 2:
                sizeOfPArray = (sizeof(pArrayMed) / sizeof(pArrayMed[0]));               // Size of process array.
                resourceCount = (sizeof(pArrayMed[1].resourceAllocation) / sizeof(int)); // Size of resources.

                printf("Amount of processes: %d\n", sizeOfPArray-1);
                printf("Amount of resources: %d\n", resourceCount);

                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                start = res.tv_nsec;

                safeSequenceCode = bankers_Algo_Avoidance(pArrayMed, sizeOfPArray, resourceCount);
                
                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                end = res.tv_nsec;
                runTime = end - start;

                print_Bankers_Algo(pArrayMed, sizeOfPArray, resourceCount);
                printf("Banker's algorithm runtime: %lfns\n", runTime);

                run_Threads(pArrayMed, sizeOfPArray-1);

                break;
            case 3:
                sizeOfPArray = (sizeof(pArrayLarge) / sizeof(pArrayLarge[0]));             // Size of process array.
                resourceCount = (sizeof(pArrayLarge[1].resourceAllocation) / sizeof(int)); // Size of resources.

                printf("Amount of processes: %d\n", sizeOfPArray-1);
                printf("Amount of resources: %d\n", resourceCount);

                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                start = res.tv_nsec;

                safeSequenceCode = bankers_Algo_Avoidance(pArrayLarge, sizeOfPArray, resourceCount);
                
                if(clock_gettime(CLOCK_REALTIME,&res) == -1)
                {
                    perror("Couldn't obtain time.\n");
                    return EXIT_FAILURE;
                }

                end = res.tv_nsec;
                runTime = end - start;

                print_Bankers_Algo(pArrayLarge, sizeOfPArray, resourceCount);
                printf("Banker's algorithm runtime: %lfns\n", runTime);

                run_Threads(pArrayLarge, sizeOfPArray-1);
                
                break;
            case 4:
                loop = 0;
                break;
            default:
                printf("Your input is out of bounds. Please try again.\n");
                break;
                continue;
        }
    }

    return 1;
}