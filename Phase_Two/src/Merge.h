#include <stdio.h>
#include <stdlib.h>

struct Process
{
    char ProcessID[13];
    int arrivalTime;
    int burstTime;
    int waitTime;
    int turnaroundTime;
    // int resourceAllocation;
    // int resourceRequest;
};

void merge(struct Process * pArray, int left, int mid, int right, int type) // Divide
{
    int i, j, k;
    int newLeftHalf = mid - left + 1;
    int newRightHalf = right - mid;

    // Allocate memory for left and right arrays
    struct Process * leftPArray = (struct Process *)malloc(newLeftHalf * sizeof(struct Process));
    struct Process * rightPArray = (struct Process *)malloc(newRightHalf * sizeof(struct Process));

    //struct Process leftPArray [newLeftHalf];
    //struct Process rightPArray [newRightHalf];

    // Copy data to left and right arrays.
    for(i = 0; i < newLeftHalf; i++)
    {
        leftPArray[i] = pArray[left + i];
    }

    for(j = 0; j < newRightHalf; j++)
    {
        rightPArray[j] = pArray[mid + 1 + j];
    }


    // Merge left and right arrays beack into
    i = 0;
    j = 0;
    k = left;

    //  Merge process

    if(type == 0)
    {
        while(i < newLeftHalf && j < newRightHalf)
        {
            // Sort relative to arrival time.
            if (leftPArray[i].arrivalTime <= rightPArray[j].arrivalTime) 
            {
                pArray[k] = leftPArray[i];
                i++;
            }
            else 
            {
                pArray[k] = rightPArray[j];
                j++;
            }
            k++;        
        }
    }else
    {
        while(i < newLeftHalf && j < newRightHalf)
        {
            // Sort relative to burst time.
            if (leftPArray[i].burstTime <= rightPArray[j].burstTime) 
            {
                pArray[k] = leftPArray[i];
                i++;
            }
            else 
            {
                pArray[k] = rightPArray[j];
                j++;
            }
            k++;        
        }
    }


    // Put remaining values if left array and right array.

    while(i < newLeftHalf)
    {
        pArray[k] = leftPArray[i];
        i++;
        k++;
    }

    while(j < newRightHalf)
    {
        pArray[k] = rightPArray[j];
        j++;
        k++;
    }

    free(leftPArray);
    free(rightPArray);
}

void mergeSort(struct Process * pArray, int left, int right, int type) // Conquer and combine
{
    // int type will tell the merge function which attributes to sort by.

    if(left < right)
    {
        int mid = left + (right - left) / 2;

        mergeSort(pArray, left, mid, type); // Sort left half.
        mergeSort(pArray, mid + 1, right, type); // Sort right half.

        merge(pArray, left, mid, right, type);
    }
}