#include <algorithm>
#include <cstring>
#include <cfloat>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <climits>
#include <deque>
#include <queue>
#include <string>
#include <iostream>
using namespace std;
typedef struct
{
    int start;
    int complete_time;
    float turnaround_time;
    float normturn_time;
    int wait_time;
    char name[10];
    int arrival_time;
    int service_time;
    int id;
    int waiting_time;
    int working_slots;
    int priority;
} process;
typedef struct
{
    int name;
    int q;
} algorithm;
int processesNum()
{
    char lines[200];
    int currline = 0;
    while (fgets(lines, sizeof(lines), stdin) != NULL)
    {
        currline++;
        if (currline == 4)
        {
            int num;
            num = atoi(lines);
            return num;
        }
    }
    printf("error happened while reading\n");
    return -1;
}
int len(string str)
{
    int length = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        length++;
    }
    return length;
}
int algoNum(string line)
{
    int num = 0;
    for (int i = 0; i < len(line); i++)
    {
        if (line[i] == ',')
        {
            num++;
        }
    }
    return num + 1;
}
void printDigits(int digit)
{
    if (digit > 9999)
        printf("%d|", digit);
    else if (digit > 999)
        printf("%d  |", digit);
    else if (digit > 99)
        printf("%d   |", digit);
    else if (digit > 9)
        printf(" %d  |", digit);
    else
        printf("  %d  |", digit);
}

// highest return ratio next
float calcHRRN(int waiting_time, int service_time)
{
    // Response Ratio = (Waiting Time + Service Time) / Service Time
    return ((float)(waiting_time + service_time)) / (float)service_time;
}
void HRRNVisualizer(process p[], int last, int noOfProcesses)
{
    int i, j, k, l;
    int schedule[noOfProcesses][last];
    memset(schedule, 0, sizeof(schedule));
    bool isInReadyQueue[noOfProcesses];
    memset(isInReadyQueue, 0, sizeof(isInReadyQueue));

    int timeSlot = 0;
    process readyQueue[noOfProcesses];
    int readyCount = 0;
    float maxHRRN;
    int selectedIndex;

    while (timeSlot < last)
    {

        for (i = 0; i < noOfProcesses; i++)
        {
            if (p[i].arrival_time == timeSlot && !isInReadyQueue[i])
            {
                readyQueue[readyCount++] = p[i];
                isInReadyQueue[i] = true;
            }
        }

        if (readyCount > 0)
        {

            maxHRRN = -1;
            selectedIndex = -1;
            for (i = 0; i < readyCount; i++)
            {
                float hrrn = calcHRRN(readyQueue[i].waiting_time, readyQueue[i].service_time);
                if (hrrn > maxHRRN)
                {
                    maxHRRN = hrrn;
                    selectedIndex = i;
                }
            }

            process *currentProcess = &readyQueue[selectedIndex];
            while (currentProcess->service_time > 0)
            {
                schedule[currentProcess->id][timeSlot++] = 1;
                currentProcess->service_time--;

                for (j = 0; j < readyCount; j++)
                {
                    if (j != selectedIndex)
                    {
                        readyQueue[j].waiting_time++;
                    }
                }

                for (j = 0; j < noOfProcesses; j++)
                {
                    if (p[j].arrival_time == timeSlot && !isInReadyQueue[j])
                    {
                        readyQueue[readyCount++] = p[j];
                        isInReadyQueue[j] = true;
                    }
                }
            }

            for (i = selectedIndex; i < readyCount - 1; i++)
            {
                readyQueue[i] = readyQueue[i + 1];
            }
            readyCount--;
        }
        else
        {
            timeSlot++;
        }
    }

    // Print schedule
    printf("HRRN  ");
    for (i = 0; i <= last; i++)
    {
        printf("%d ", i % 10);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-\n");

    for (k = 0; k < noOfProcesses; k++)
    {
        int firstExecution = -1;
        for (l = 0; l < last; l++)
        {
            if (schedule[k][l] == 1)
            {
                firstExecution = l;
                break;
            }
        }
        printf("%s     ", p[k].name);
        for (l = 0; l < last; l++)
        {
            printf("|");

            if (schedule[k][l] == 1)
            {
                printf("*"); // Execution
            }
            else if (l >= p[k].arrival_time && (firstExecution == -1 || l < firstExecution))
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("| ");
        printf("\n");
    }

    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n\n");
}
void HRRNstatistics(process processes[], char *status, int processNum, int lastInstance)
{
    if (strcmp(status, "stats") == 0)
    {
        float totalTurnaroundTime = 0;
        float totalNormTurnaroundTime = 0;
        float turnaroundMean = 0;
        float normTurnaroundMean = 0;
        int i = 0;
        int currentTime = 0;
        int completedProcesses = 0;

        int completed[processNum];
        memset(completed, 0, sizeof(completed));

        while (completedProcesses < processNum)
        {
            float maxHRRN = -1;
            int selectedIndex = -1;

            for (i = 0; i < processNum; i++)
            {
                if (!completed[i] && processes[i].arrival_time <= currentTime)
                {
                    int waitingTime = currentTime - processes[i].arrival_time;
                    float hrrn = calcHRRN(waitingTime, processes[i].service_time);
                    if (hrrn > maxHRRN)
                    {
                        maxHRRN = hrrn;
                        selectedIndex = i;
                    }
                }
            }

            if (selectedIndex != -1)
            {
                process *currentProcess = &processes[selectedIndex];
                currentTime += currentProcess->service_time;
                currentProcess->complete_time = currentTime;
                currentProcess->turnaround_time = currentProcess->complete_time - currentProcess->arrival_time;
                currentProcess->normturn_time = currentProcess->turnaround_time / (float)currentProcess->service_time;

                totalTurnaroundTime += currentProcess->turnaround_time;
                totalNormTurnaroundTime += currentProcess->normturn_time;

                completed[selectedIndex] = 1;
                completedProcesses++;
            }
            else
            {
                currentTime++;
            }
        }

        turnaroundMean = totalTurnaroundTime / processNum;
        normTurnaroundMean = totalNormTurnaroundTime / processNum;

        printf("HRRN\n");
        printf("Process    |");
        for (i = 0; i < processNum; i++)
        {
            printf("  %s  |", processes[i].name);
        }
        printf("\nArrival    |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].arrival_time);
        }
        printf("\nService    |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].service_time);
        }
        printf(" Mean|\n");
        printf("Finish     |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].complete_time);
        }
        printf("-----|\n");
        printf("Turnaround |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].turnaround_time);
        }
        if (turnaroundMean > 9.99)
            printf("%0.2f|", turnaroundMean);
        else
            printf(" %0.2f|", turnaroundMean);
        printf("\nNormTurn   |");
        for (i = 0; i < processNum; i++)
        {
            printf(" %0.2f|", processes[i].normturn_time);
        }
        if (normTurnaroundMean > 9.99)
            printf("%0.2f|\n", normTurnaroundMean);
        else
            printf(" %0.2f|\n", normTurnaroundMean);
    }
    printf("\n");
}

// shortest remian time
void SRTVisualizer(process p[], int last, int noOfProcesses)
{
    printf("SRT   ");
    int i, j;
    for (i = 0; i <= last; i++)
    {
        printf("%d ", i % 10);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-\n");

    int schedule[noOfProcesses][last];
    for (i = 0; i < noOfProcesses; i++)
    {
        for (j = 0; j < last; j++)
        {
            schedule[i][j] = 0;
        }
    }

    int timeSlot = 0;

    process *readyQueue = (process *)malloc(noOfProcesses * sizeof(process));
    int readyQueueSize = 0;

    int readyStart[noOfProcesses];
    int readyEnd[noOfProcesses];
    for (i = 0; i < noOfProcesses; i++)
    {
        readyStart[i] = -1;
        readyEnd[i] = -1;
    }

    while (timeSlot < last)
    {
        for (i = 0; i < noOfProcesses; i++)
        {
            if (p[i].arrival_time == timeSlot)
            {
                readyQueue[readyQueueSize++] = p[i];
                if (readyStart[i] == -1)
                {
                    readyStart[i] = timeSlot;
                }
            }
        }

        if (readyQueueSize > 0)
        {
            int minIndex = 0;
            for (i = 1; i < readyQueueSize; i++)
            {
                if (readyQueue[i].service_time < readyQueue[minIndex].service_time)
                {
                    minIndex = i;
                }
            }

            schedule[readyQueue[minIndex].id][timeSlot] = 1;
            readyQueue[minIndex].service_time--;

            if (readyQueue[minIndex].service_time <= 0)
            {
                readyEnd[readyQueue[minIndex].id] = timeSlot;
                for (i = minIndex; i < readyQueueSize - 1; i++)
                {
                    readyQueue[i] = readyQueue[i + 1];
                }
                readyQueueSize--;
            }
        }
        timeSlot++;
    }
    int k, l;
    for (k = 0; k < noOfProcesses; k++)
    {
        printf("%s     ", p[k].name);
        for (l = 0; l <= last; l++)
        {
            printf("|");
            if (schedule[k][l] == 1)
            {
                printf("*");
            }
            else if (readyStart[k] <= l && (readyEnd[k] == -1 || l < readyEnd[k]))
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-\n");
    printf("\n");

    free(readyQueue);
}
void SRTstatistics(process processes[], char *status, int processNum, int lastInstance)
{
    if (strcmp(status, "stats") == 0)
    {
        float normMean = 0;
        float totTurnaroundTime = 0;
        float turnaroundMean = 0;
        int i = 0;
        int remainingTime[processNum];
        int completed[processNum];
        memset(completed, 0, sizeof(completed));

        for (i = 0; i < processNum; i++)
        {
            remainingTime[i] = processes[i].service_time;
        }

        int time = 0;
        int completedProcesses = 0;

        while (completedProcesses < processNum)
        {
            int minIndex = -1;
            int minRemainingTime = INT_MAX;

            for (i = 0; i < processNum; i++)
            {
                if (!completed[i] && processes[i].arrival_time <= time && remainingTime[i] < minRemainingTime)
                {
                    minRemainingTime = remainingTime[i];
                    minIndex = i;
                }
            }

            if (minIndex != -1)
            {
                remainingTime[minIndex]--;

                if (remainingTime[minIndex] == 0)
                {
                    completed[minIndex] = 1;
                    completedProcesses++;

                    processes[minIndex].complete_time = time + 1;
                    processes[minIndex].turnaround_time = processes[minIndex].complete_time - processes[minIndex].arrival_time;
                    processes[minIndex].normturn_time = (float)processes[minIndex].turnaround_time / processes[minIndex].service_time;

                    totTurnaroundTime += processes[minIndex].turnaround_time;
                    normMean += processes[minIndex].normturn_time;
                }
            }

            time++;
        }

        turnaroundMean = totTurnaroundTime / processNum;

        printf("SRT\n");
        printf("Process    |");
        for (i = 0; i < processNum; i++)
        {
            printf("  %s  |", processes[i].name);
        }
        printf("\nArrival    |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].arrival_time);
        }
        printf("\nService    |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].service_time);
        }
        printf(" Mean|\n");
        printf("Finish     |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].complete_time);
        }
        printf("-----|\n");
        printf("Turnaround |");
        for (i = 0; i < processNum; i++)
        {
            printDigits(processes[i].turnaround_time);
        }
        if (turnaroundMean > 9.99)
            printf("%0.2f|", turnaroundMean);
        else
            printf(" %0.2f|", turnaroundMean);
        printf("\nNormTurn   |");
        for (i = 0; i < processNum; i++)
        {
            printf(" %0.2f|", processes[i].normturn_time);
        }
        if ((float)normMean / (float)processNum > 9.99)
            printf("%0.2f|\n", (float)normMean / (float)processNum);
        else
            printf(" %0.2f|\n", (float)normMean / (float)processNum);
    }
    printf("\n");
}

// round robin
void RoundRobinVisualizer(process p[], int q, int last, int noOfProcesses)
{
    std::queue<process> readyQueue;

    int schedule[noOfProcesses][last];
    memset(schedule, 0, sizeof(schedule));

    int currSlot = 0;
    int processIndex = 0;

    // Add processes arriving at time 0
    process firstProcess = p[processIndex];
    if (processIndex < noOfProcesses && firstProcess.arrival_time <= currSlot)
    {
        readyQueue.push(firstProcess);
        processIndex++;
    }

    while (currSlot < last)
    {
        if (!readyQueue.empty())
        {
            process pr = readyQueue.front();
            readyQueue.pop();

            int i;
            for (i = 0; i < q && pr.service_time > 0 && currSlot < last; i++)
            {
                schedule[pr.id][currSlot] = 1;
                pr.service_time--;
                currSlot++;

                process nextProcess = p[processIndex];
                if (processIndex < noOfProcesses && nextProcess.arrival_time <= currSlot)
                {
                    readyQueue.push(nextProcess);
                    processIndex++;
                }
            }

            if (pr.service_time > 0)
            {
                readyQueue.push(pr);
            }
        }
        else
        {
            currSlot++;
            process nextProcess = p[processIndex];
            if (processIndex < noOfProcesses && nextProcess.arrival_time <= currSlot)
            {
                readyQueue.push(nextProcess);
                processIndex++;
            }
        }
    }

    process pp[noOfProcesses];
    int k;
    for (k = 0; k < noOfProcesses; k++)
    {
        pp[k] = p[k];
    }

    printf("RR-%d  ", q);
    int i;
    for (i = 0; i <= last; i++)
    {
        printf("%d ", i % 10);
    }
    printf("\n");

    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n");

    int l;
    for (k = 0; k < noOfProcesses; k++)
    {
        printf("%s     ", pp[k].name);
        for (l = 0; l < last; l++)
        {
            printf("|");
            if (schedule[k][l] == 1)
            {
                printf("*");
                pp[k].service_time--;
            }
            else if (pp[k].arrival_time <= l && pp[k].service_time != 0)
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("| ");
        printf("\n");
    }

    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n\n");
}
void RRstatistics(process processes[], int q, int processNum)
{
    float normMean = 0;
    float totTurnaroundTime = 0;
    float turnaroundMean = 0;

    int remainingTime[processNum];
    int completed[processNum];
    memset(completed, 0, sizeof(completed));

    int i;
    for (i = 0; i < processNum; i++)
    {
        remainingTime[i] = processes[i].service_time;
    }

    int time = 0;
    int completedProcesses = 0;
    std::queue<int> readyQueue;

    for (i = 0; i < processNum; i++)
    {
        if (processes[i].arrival_time == 0)
        {
            readyQueue.push(i);
        }
    }

    while (completedProcesses < processNum)
    {
        if (!readyQueue.empty())
        {
            int currentIndex = readyQueue.front();
            readyQueue.pop();

            int execTime = (remainingTime[currentIndex] < q) ? remainingTime[currentIndex] : q;
            remainingTime[currentIndex] -= execTime;
            time += execTime;

            for (i = 0; i < processNum; i++)
            {
                if (processes[i].arrival_time > time - execTime && processes[i].arrival_time <= time && !completed[i])
                {
                    readyQueue.push(i);
                }
            }

            if (remainingTime[currentIndex] > 0)
            {
                readyQueue.push(currentIndex);
            }
            else
            {
                completed[currentIndex] = 1;
                completedProcesses++;
                processes[currentIndex].complete_time = time;
                processes[currentIndex].turnaround_time = processes[currentIndex].complete_time - processes[currentIndex].arrival_time;
                processes[currentIndex].normturn_time = (float)processes[currentIndex].turnaround_time / processes[currentIndex].service_time;

                totTurnaroundTime += processes[currentIndex].turnaround_time;
                normMean += processes[currentIndex].normturn_time;
            }
        }
        else
        {
            time++;
        }
    }

    turnaroundMean = totTurnaroundTime / processNum;

    printf("RR-%d\n", q);
    printf("Process    |");
    for (i = 0; i < processNum; i++)
    {
        printf("  %s  |", processes[i].name);
    }
    printf("\nArrival    |");
    for (i = 0; i < processNum; i++)
    {
        printDigits(processes[i].arrival_time);
    }
    printf("\nService    |");
    for (i = 0; i < processNum; i++)
    {
        printDigits(processes[i].service_time);
    }
    printf(" Mean|\n");
    printf("Finish     |");
    for (i = 0; i < processNum; i++)
    {
        printDigits(processes[i].complete_time);
    }
    printf("-----|\n");
    printf("Turnaround |");
    for (i = 0; i < processNum; i++)
    {
        printDigits((int)processes[i].turnaround_time);
    }
    if (turnaroundMean > 9.99)
        printf("%0.2f|", turnaroundMean);
    else
        printf(" %0.2f|", turnaroundMean);
    printf("\nNormTurn   |");
    for (i = 0; i < processNum; i++)
    {
        printf(" %0.2f|", processes[i].normturn_time);
    }
    if ((float)normMean / (float)processNum > 9.99)
        printf("%0.2f|\n", (float)normMean / (float)processNum);
    else
        printf(" %0.2f|\n", (float)normMean / (float)processNum);
    printf("\n");
}

void FCFSvisualizer(process processes[], int lastInstance, int processNum)
{
    printf("FCFS  ");
    // int currtime = 0;
    int busytime[lastInstance];
    int i;
    for (i = 0; i < lastInstance; i++)
    {
        busytime[i] = 0;
    }
    for (i = 0; i <= lastInstance; i++)
    {
        int t = i % 10;
        printf("%d ", t);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n");

    process queue[processNum];
    int start = 0;
    int end = 0;
    int j;
    for (i = 0; i <= lastInstance; i++)
    {
        for (j = 0; j < processNum; j++)
        {
            if (processes[j].arrival_time == i)
            {
                queue[end] = processes[j];
                end = end + 1;
            }
        }
    }
    process pr;
    while (start < end)
    {
        pr = queue[start];
        start = start + 1;
        printf("%s     ", pr.name);
        for (i = 0; i <= lastInstance; i++)
        {
            printf("|");
            if (pr.service_time != 0 && (pr.arrival_time <= i))
            {
                if (busytime[i] == 0)
                {
                    printf("*");
                    busytime[i] = 1;
                    pr.service_time = pr.service_time - 1;
                }
                else
                {
                    printf(".");
                    pr.arrival_time = pr.arrival_time + 1;
                }
            }
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }
    printf("-");
    // printf("\n");
}
void FCFSstatistics(process processes[], char *status, int processNum, int lastInstance)
{
    if (strcmp(status, "stats") == 0)
    {
        float normMean = 0;
        float totTurnaroundTime = 0;
        float turnaroundMean = 0;
        processes[0].complete_time = processes[0].service_time;
        processes[0].turnaround_time = processes[0].complete_time;
        processes[0].normturn_time = processes[0].turnaround_time / processes[0].service_time;
        totTurnaroundTime += processes[0].turnaround_time;
        int i;
        for (i = 1; i < processNum; i++)
        {
            processes[i].complete_time = processes[i - 1].complete_time + processes[i].service_time;
            processes[i].turnaround_time = processes[i].complete_time - processes[i].arrival_time;
            processes[i].normturn_time = processes[i].turnaround_time / processes[i].service_time;
            totTurnaroundTime += processes[i].turnaround_time; // Update total turnaround time
        }
        turnaroundMean = totTurnaroundTime / processNum;
        process pr[processNum];
        for (i = 0; i < processNum; i++)
        {
            pr[i] = processes[i];
        }
        printf("FCFS\n");
        printf("Process    |");
        for (i = 0; i < processNum; i++)
        {
            printf("  %s  |", pr[i].name);
        }
        printf("\nArrival    |");
        int digit;
        for (i = 0; i < processNum; i++)
        {
            digit = pr[i].arrival_time;
            printDigits(digit);
        }
        printf("\nService    |");
        for (i = 0; i < processNum; i++)
        {
            digit = pr[i].service_time;
            printDigits(digit);
        }
        printf(" Mean|");
        printf("\nFinish     |");
        for (i = 0; i < processNum; i++)
        {
            digit = pr[i].complete_time;
            printDigits(digit);
        }
        printf("-----|");
        printf("\nTurnaround |");
        for (i = 0; i < processNum; i++)
        {
            digit = pr[i].turnaround_time;
            printDigits(digit);
        }
        if (turnaroundMean > 9.99)
            printf("%0.2f|", turnaroundMean);
        else
            printf(" %0.2f|", turnaroundMean);
        printf("\nNormTurn   |");
        for (i = 0; i < processNum; i++)
        {
            normMean = normMean + (round(pr[i].normturn_time * 100) / 100);
            printf(" %0.2f|", pr[i].normturn_time);
        }
        if ((float)normMean / (float)processNum > 9.99)
        {
            printf("%0.2f|\n", (float)normMean / (float)processNum);
        }
        else
        {
            printf(" %0.2f|", (float)normMean / (float)processNum);
        }
    }
}

void SPNvisualizer(process processes[], int lastInstance, int processNum)
{
    int schedule[processNum][lastInstance];
    int i;
    int j;
    for (i = 0; i < processNum; i++)
    {
        for (j = 0; j < lastInstance; j++)
        {
            schedule[i][j] = 0;
        }
    }
    int currtime = 0;
    int processIndex = 0;
    process queue[processNum];
    int size = 0;
    for (i = 0; i < processNum; i++)
    {
        if (processes[i].arrival_time == currtime)
        {
            queue[size] = processes[processIndex];
            size = size + 1;
        }
    }
    int min;
    int currIndex;
    while (currtime < lastInstance)
    {
        if (size > 0)
        {
            min = queue[0].service_time;
            currIndex = 0;
            for (i = 1; i < size; i++)
            {
                if (queue[i].service_time < min)
                {
                    min = queue[i].service_time;
                    currIndex = i;
                }
            }
            while (queue[currIndex].service_time != 0)
            {
                schedule[queue[currIndex].id][currtime] = 1;
                queue[currIndex].service_time = queue[currIndex].service_time - 1;
                currtime = currtime + 1;
                for (i = 0; i < processNum; i++)
                {
                    if (processes[i].arrival_time == currtime)
                    {
                        queue[size] = processes[i];
                        size = size + 1;
                    }
                }
            }
            for (i = currIndex; i < size - 1; i++)
            {

                queue[i] = queue[i + 1];
            }
            size = size - 1;
        }
        else
        {
            currtime = currtime + 1;
            if (processIndex < lastInstance && processes[processIndex].arrival_time <= currtime)
            {
                queue[size] = processes[processIndex];
                size = size + 1;
                processIndex = processIndex + 1;
            }
        }
    }
    process pr[processNum];
    for (i = 0; i < processNum; i++)
    {
        pr[i] = processes[i];
    }
    printf("SPN   ");
    for (i = 0; i <= lastInstance; i++)
    {
        int t = i % 10;
        printf("%d ", t);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n");
    for (i = 0; i < processNum; i++)
    {
        printf("%s     ", pr[i].name);
        for (j = 0; j < lastInstance; j++)
        {
            printf("|");
            if (schedule[i][j] == 1)
            {
                printf("*");
                pr[i].service_time = pr[i].service_time - 1;
            }
            else if (pr[i].arrival_time <= j && pr[i].service_time != 0)
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("|");
        printf(" \n");
    }
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }

    printf("-");
    // printf("\n");
}
void SPNstatistics(process processes[], char *status, int processNum, int lastInstance)
{
    float normMean = 0;
    float totTurnaroundTime = 0;
    float turnaroundMean = 0;
    int schedule[processNum][lastInstance];
    int i;
    int j;
    for (i = 0; i < processNum; i++)
    {
        for (j = 0; j < lastInstance; j++)
        {
            schedule[i][j] = 0;
        }
    }
    int currtime = 0;
    int processIndex = 0;
    process queue[processNum];
    int size = 0;
    for (i = 0; i < processNum; i++)
    {
        if (processes[i].arrival_time == currtime)
        {
            queue[size] = processes[processIndex];
            size = size + 1;
        }
    }
    int min;
    int currIndex = 0;
    while (currtime < lastInstance)
    {
        if (size > 0)
        {
            min = queue[0].service_time;
            currIndex = 0;
            for (i = 1; i < size; i++)
            {
                if (queue[i].service_time < min)
                {
                    min = queue[i].service_time;
                    currIndex = i;
                }
            }
            while (queue[currIndex].service_time != 0)
            {
                schedule[queue[currIndex].id][currtime] = 1;
                queue[currIndex].service_time = queue[currIndex].service_time - 1;
                currtime = currtime + 1;
                for (i = 0; i < processNum; i++)
                {
                    if (processes[i].arrival_time == currtime)
                    {
                        queue[size] = processes[i];
                        size = size + 1;
                    }
                }
            }
            for (i = currIndex; i < size - 1; i++)
            {
                queue[i] = queue[i + 1];
            }
            size = size - 1;
        }
        else
        {
            currtime = currtime + 1;
            if (processIndex < lastInstance && processes[processIndex].arrival_time <= currtime)
            {
                queue[size] = processes[processIndex];
                size = size + 1;
                processIndex = processIndex + 1;
            }
        }
    }
    int temp;
    for (i = 0; i < processNum; i++)
    {
        temp = 0;
        for (j = 0; j < lastInstance; j++)
        {
            if (schedule[i][j] == 1)
            {
                temp = j;
            }
        }
        processes[i].complete_time = temp + 1;
        processes[i].turnaround_time = processes[i].complete_time - processes[i].arrival_time;
        processes[i].normturn_time = processes[i].turnaround_time / processes[i].service_time;
        totTurnaroundTime += processes[i].turnaround_time;
    }
    turnaroundMean = totTurnaroundTime / processNum;
    process pr[processNum];
    for (i = 0; i < processNum; i++)
    {
        pr[i] = processes[i];
    }
    printf("SPN\n");
    printf("Process    |");
    for (i = 0; i < processNum; i++)
        printf("  %s  |", pr[i].name);
    printf("\nArrival    |");
    int digit;
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].arrival_time;
        printDigits(digit);
    }
    printf("\nService    |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].service_time;
        printDigits(digit);
    }
    printf(" Mean|");
    printf("\nFinish     |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].complete_time;
        printDigits(digit);
    }
    printf("-----|");
    printf("\nTurnaround |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].turnaround_time;
        printDigits(digit);
    }
    if (turnaroundMean > 9.99)
        printf("%0.2f|", turnaroundMean);
    else
        printf(" %0.2f|", turnaroundMean);
    printf("\nNormTurn   |");
    for (i = 0; i < processNum; i++)
    {
        normMean = normMean + (round(pr[i].normturn_time * 100) / 100);
        printf(" %0.2f|", pr[i].normturn_time);
    }
    if ((float)normMean / (float)processNum > 9.99)
    {
        printf("%0.2f|\n", (float)normMean / (float)processNum);
    }
    else
    {
        printf(" %0.2f|", (float)normMean / (float)processNum);
    }
}
int FBProcessesNum(deque<deque<process>> queue)
{
    int processNum = 0;
    int i;
    for (i = 0; i < queue.size(); i++)
    {
        bool isempty = queue[i].empty();
        if (!isempty)
        {
            processNum = processNum + queue[i].size();
        }
    }
    return processNum;
}
void FBvisualizer(process processes[], int q, int lastInstance, int processNum)
{

    int isQpow2 = 0;
    if (q == 0)
    {
        isQpow2 = 1;
    }
    int schedule[processNum][lastInstance];
    int i;
    int j;
    for (i = 0; i < processNum; i++)
    {
        for (j = 0; j < lastInstance; j++)
        {
            schedule[i][j] = 0;
        }
    }
    deque<deque<process>> queue(processNum);
    int currtime = 0;
    int processIndex = 0;
    int currq = -1;
    if (processIndex < processNum && processes[processIndex].arrival_time <= currtime)
    {
        queue[0].push_back(processes[processIndex]);
        processIndex = processIndex + 1;
    }
    while (currtime < lastInstance)
    {
        for (i = 0; i < processNum; i++)
        {
            bool isempty = queue[i].empty();
            if (!isempty)
            {
                currq = i;
                break;
            }
        }
        if (currq == -1)
        {
            currtime = currtime + 1;
            continue;
        }
        process firstpr = queue[currq].front();
        if (isQpow2 == 1)
        {
            q = pow(2, currq);
        }
        for (i = 0; i < q && firstpr.service_time > 0; i++)
        {
            schedule[firstpr.id][currtime] = 1;
            firstpr.service_time = firstpr.service_time - 1;
            currtime = currtime + 1;
            if (processIndex < processNum && processes[processIndex].arrival_time <= currtime)
            {
                queue[0].push_back(processes[processIndex]);
                processIndex = processIndex + 1;
            }
        }
        queue[currq].pop_front();
        if (firstpr.service_time > 0)
        {
            int checkq = currq + 1;
            int checkprc = FBProcessesNum(queue) + 1;
            if (checkq < processNum && checkprc > 1)
            {
                queue[checkq].push_back(firstpr);
            }
            else
            {
                queue[currq].push_back(firstpr);
            }
        }
    }
    process pr[processNum];
    for (i = 0; i < processNum; i++)
    {
        pr[i] = processes[i];
    }
    if (isQpow2 == 0)
    {
        printf("FB-1  ");
    }
    else
    {
        printf("FB-2i ");
    }
    for (i = 0; i <= lastInstance; i++)
    {
        int t = i % 10;
        printf("%d ", t);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n");
    for (i = 0; i < processNum; i++)
    {
        printf("%s     ", pr[i].name);
        for (j = 0; j < lastInstance; j++)
        {
            printf("|");
            if (schedule[i][j] == 1)
            {
                printf("*");
                pr[i].service_time = pr[i].service_time - 1;
            }
            else if (pr[i].arrival_time <= j && pr[i].service_time != 0)
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("|");
        printf(" \n");
    }
    printf("-----");
    for (i = 0; i <= lastInstance; i++)
    {
        printf("--");
    }

    printf("-");
}
void FBstatistics(process processes[], int q, int lastInstance, int processNum)
{
    float normMean = 0;
    float totTurnaroundTime = 0;
    float turnaroundMean = 0;
    int schedule[processNum][lastInstance];
    int i;
    int j;
    for (i = 0; i < processNum; i++)
    {
        for (j = 0; j < lastInstance; j++)
        {
            schedule[i][j] = 0;
        }
    }
    int currtime = 0;
    int processIndex = 0;
    int isQpow2 = 0;
    if (q == 0)
    {
        isQpow2 = 1;
    }
    deque<deque<process>> queue(processNum);
    int currq = -1;
    if (processIndex < processNum && processes[processIndex].arrival_time <= currtime)
    {
        queue[0].push_back(processes[processIndex]);
        processIndex = processIndex + 1;
    }
    while (currtime < lastInstance)
    {
        for (i = 0; i < processNum; i++)
        {
            bool isempty = queue[i].empty();
            if (!isempty)
            {
                currq = i;
                break;
            }
        }
        if (currq == -1)
        {
            currtime = currtime + 1;
            continue;
        }
        process firstpr = queue[currq].front();
        if (isQpow2 == 1)
        {
            q = pow(2, currq);
        }
        for (i = 0; i < q && firstpr.service_time > 0; i++)
        {
            schedule[firstpr.id][currtime] = 1;
            firstpr.service_time = firstpr.service_time - 1;
            currtime = currtime + 1;
            if (processIndex < processNum && processes[processIndex].arrival_time <= currtime)
            {
                queue[0].push_back(processes[processIndex]);
                processIndex = processIndex + 1;
            }
        }
        queue[currq].pop_front();
        if (firstpr.service_time > 0)
        {
            int checkq = currq + 1;
            int checkprc = FBProcessesNum(queue) + 1;
            if (checkq < processNum && checkprc > 1)
            {
                queue[checkq].push_back(firstpr);
            }
            else
            {
                queue[currq].push_back(firstpr);
            }
        }
    }
    int temp;
    for (i = 0; i < processNum; i++)
    {
        temp = 0;
        for (j = 0; j < lastInstance; j++)
        {
            if (schedule[i][j] == 1)
            {
                temp = j;
            }
        }
        processes[i].complete_time = temp + 1;
        processes[i].turnaround_time = processes[i].complete_time - processes[i].arrival_time;
        processes[i].normturn_time = processes[i].turnaround_time / processes[i].service_time;
        totTurnaroundTime += processes[i].turnaround_time;
    }
    turnaroundMean = totTurnaroundTime / processNum;
    process pr[processNum];
    for (i = 0; i < processNum; i++)
    {
        pr[i] = processes[i];
    }
    if (isQpow2 == 0)
    {
        printf("FB-1\n");
    }
    else
    {
        printf("FB-2i\n");
    }
    printf("Process    |");
    for (i = 0; i < processNum; i++)
        printf("  %s  |", pr[i].name);
    printf("\nArrival    |");
    int digit;
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].arrival_time;
        printDigits(digit);
    }
    printf("\nService    |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].service_time;
        printDigits(digit);
    }
    printf(" Mean|");
    printf("\nFinish     |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].complete_time;
        printDigits(digit);
    }
    printf("-----|");
    printf("\nTurnaround |");
    for (i = 0; i < processNum; i++)
    {
        digit = pr[i].turnaround_time;
        printDigits(digit);
    }
    if (turnaroundMean > 9.99)
        printf("%0.2f|", turnaroundMean);
    else
        printf(" %0.2f|", turnaroundMean);
    printf("\nNormTurn   |");
    for (i = 0; i < processNum; i++)
    {
        normMean = normMean + (round(pr[i].normturn_time * 100) / 100);
        printf(" %0.2f|", pr[i].normturn_time);
    }
    if ((float)normMean / (float)processNum > 9.99)
    {
        printf("%0.2f|\n", (float)normMean / (float)processNum);
    }
    else
    {
        printf(" %0.2f|", (float)normMean / (float)processNum);
    }
}

// aging
void AgingVisualizer(process p[], int q, int last, int noOfProcesses)
{
    int schedule[noOfProcesses][last];
    memset(schedule, 0, sizeof(schedule));

    std::vector<process> readyQueue;
    process currentProcess;
    currentProcess.id = 0;
    currentProcess.arrival_time = 0;
    currentProcess.priority = INT_MIN;
    memset(currentProcess.name, '\0', sizeof(currentProcess.name));

    char originalPriorities[noOfProcesses][2][50];
    for (int i = 0; i < noOfProcesses; i++)
    {
        strncpy(originalPriorities[i][0], p[i].name, sizeof(originalPriorities[i][0]) - 1);
        originalPriorities[i][0][sizeof(originalPriorities[i][0]) - 1] = '\0';
        snprintf(originalPriorities[i][1], sizeof(originalPriorities[i][1]), "%d", p[i].priority);
    }

    bool processRunning = false;
    int timeSlot = 0;
    int currentQuantum = 0;
    int i, l, j,k;

    while (timeSlot < last)
    {
        for (i = 0; i < noOfProcesses; i++)
        {
            if (p[i].arrival_time <= timeSlot)
            {
                bool alreadyInQueue = false;
                for (int j = 0; j < readyQueue.size(); j++)
                {
                    const auto &proc = readyQueue[j];
                    if (proc.id == p[i].id)
                    {
                        alreadyInQueue = true;
                        break;
                    }
                }

                if (!alreadyInQueue && p[i].id != currentProcess.id)
                {
                    readyQueue.push_back(p[i]);
                }
            }
        }

        // for ( j = 0; j < readyQueue.size(); j++)
        // {
        //     if (readyQueue[j].id != currentProcess.id)
        //     {
        //         readyQueue[j].priority++;
        //     }
        // }

        if (!processRunning && readyQueue.empty())
        {
            for (i = 0; i < noOfProcesses; i++)
            {
                if (p[i].arrival_time <= timeSlot)
                {
                    currentProcess = p[i];
                    processRunning = true;
                    break;
                }
            }
        }

        if (processRunning)
        {
            schedule[currentProcess.id][timeSlot] = 1;
            currentQuantum++;
            timeSlot++;

            if (currentQuantum == q)
            {
                for (i = 0; i < noOfProcesses; i++)
                {
                    if (currentProcess.id == p[i].id)
                    {
                        currentProcess.priority = atoi(originalPriorities[i][1]);
                        break;
                    }
                }
                readyQueue.push_back(currentProcess);
                processRunning = false;
                currentQuantum = 0;
            }
        }
        else if (!readyQueue.empty())
        {
            for ( j = 0; j < readyQueue.size(); j++)
        {
            if (readyQueue[j].id != currentProcess.id)
            {
                readyQueue[j].priority++;
            }
        }
            std::sort(readyQueue.begin(), readyQueue.end(), [&](const process &a, const process &b)
                      { return a.priority > b.priority; });

            currentProcess = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            processRunning = true;
        }
        else
        {
            timeSlot++;
        }
    }

    printf("Aging ");
    for (i = 0; i <= last; i++)
    {
        printf("%d ", i % 10);
    }
    printf("\n");
    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-");
    printf("\n");
    for (k = 0; k < noOfProcesses; k++)
    {
        printf("%s     ", p[k].name);
        for (l = 0; l < last; l++)
        {
            printf("|");
            if (schedule[k][l] == 1)
            {
                printf("*");
            }
            else if (l >= p[k].arrival_time && schedule[k][l] == 0)
            {
                printf(".");
            }
            else
            {
                printf(" ");
            }
        }
        printf("| \n");
    }

    printf("-----");
    for (i = 0; i <= last; i++)
    {
        printf("--");
    }
    printf("-");
}

int main()
{
    char status[10];
    scanf("%s", status);
    string line;
    cin >> line;
    /*algorithm algo;
    char *tempa;

    tempa = strchr(line, '-');
    if (tempa != NULL)
    {
        sscanf(line, "%d-%d", &algo.name, &algo.q);
    }
    else
    {
        algo.name = atoi(line);
        algo.q = 0;
    }*/
    int numAlgo = algoNum(line);
    int countAlgo = 0;
    algorithm algo[numAlgo];

    int currIndex = 0, i = 0;
    int startIndex = 0, endIndex = 0;
    string subStr;
    int tokenCounter = 0;
    int currSub = 0, iSub = 0;
    int startSub = 0, endSub = 0;
    string subsub;
    int tokenCounterSub = 0;

    if (len(line) == 1)
    {
        algo[countAlgo].name = stoi(line);
        algo[countAlgo].q = 0;
        countAlgo++;
    }
    else
    {
        if (!(line.find(",") != std::string::npos))
        {
            while (i <= len(line))
            {
                if (line[i] == '-' || i == len(line))
                {
                    endIndex = i;
                    subStr = "";
                    subStr.append(line, startIndex, endIndex - startIndex);
                    tokenCounter = tokenCounter + 1;
                    if (tokenCounter == 1)
                    {
                        algo[countAlgo].name = stoi(subStr);
                    }
                    if (tokenCounter == 2)
                    {
                        algo[countAlgo].q = stoi(subStr);
                    }
                    currIndex = currIndex + 1;
                    startIndex = endIndex + 1;
                }
                i = i + 1;
            }
            countAlgo = countAlgo + 1;
            currIndex = 0;
            startIndex = 0;
            endIndex = 0;
            i = 0;
            tokenCounter = 0;
        }
        else
        {
            while (i <= len(line))
            {
                if (line[i] == ',' || i == len(line))
                {
                    endIndex = i;
                    subStr = "";
                    subStr.append(line, startIndex, endIndex - startIndex);
                    if (len(subStr) == 1)
                    {
                        algo[countAlgo].name = stoi(subStr);
                        algo[countAlgo].q = 0;
                        countAlgo = countAlgo + 1;
                    }
                    else
                    {
                        while (iSub <= len(subStr))
                        {
                            if (subStr[iSub] == '-' || iSub == len(subStr))
                            {
                                endSub = iSub;
                                subsub = "";
                                subsub.append(subStr, startSub, endSub - startSub);
                                tokenCounterSub++;
                                if (tokenCounterSub == 1)
                                {
                                    algo[countAlgo].name = stoi(subsub);
                                }
                                if (tokenCounterSub == 2)
                                {
                                    algo[countAlgo].q = stoi(subsub);
                                }
                                currSub = currSub + 1;
                                startSub = endSub + 1;
                            }
                            iSub = iSub + 1;
                        }
                        countAlgo = countAlgo + 1;
                        currSub = 0;
                        startSub = 0;
                        endSub = 0;
                        iSub = 0;
                        tokenCounterSub = 0;
                    }
                    currIndex = currIndex + 1;
                    startIndex = endIndex + 1;
                }
                i = i + 1;
            }
            currIndex = 0;
            startIndex = 0;
            endIndex = 0;
            i = 0;
            tokenCounter = 0;
        }
    }

    int lastInstance;
    scanf("%d", &lastInstance);
    int processNum;
    scanf("%d", &processNum);
    process processes[processNum];
    // printf("%s %s %d %d %d %d\n",status,line,algo.name,algo.q,lastInstance,processNum);
    char temp[100];
    int id = 0;
    int countProcess = 0;
    int j;
    for (j = 0; j < processNum; j++)
    {
        scanf("%s", temp);
        int tokenCounter = 0;
        char subStr[100];
        int startIndex = 0;
        int endIndex = 0;
        for (int i = 0; i <= strlen(temp); i++)
        {

            if (temp[i] == ',' || i == strlen(temp))
            {
                endIndex = i;
                strncpy(subStr, temp + startIndex, endIndex - startIndex);
                subStr[endIndex - startIndex] = '\0';
                tokenCounter++;
                if (tokenCounter == 1)
                {
                    strncpy(processes[countProcess].name, subStr, sizeof(processes[countProcess].name) - 1);
                    processes[countProcess].name[sizeof(processes[countProcess].name) - 1] = '\0';
                }
                if (tokenCounter == 2)
                {
                    processes[countProcess].arrival_time = atoi(subStr);
                }
                if (tokenCounter == 3)
                {
                    processes[countProcess].service_time = atoi(subStr);
                    processes[countProcess].priority = atoi(subStr);
                    processes[countProcess].id = id++;
                    processes[countProcess].working_slots = 0;
                    processes[countProcess].waiting_time = 0;
                }
                startIndex = endIndex + 1;
            }
        }
        // printf("process %d name %s arrival %d service %d id %d\n", countProcess, processes[countProcess].name,processes[countProcess].arrival_time, processes[countProcess].service_time, processes[countProcess].id);
        countProcess = countProcess + 1;
    }
    /*for (int i = 0; i < countAlgo; i++)
    {
        printf("algo %d id = %d q = %d\n", i, algo[i].name, algo[i].q);
    }*/
    if (strcmp(status, "trace") == 0)
    {
        // for loop hena l more than one algorithm
        for (int i = 0; i < countAlgo; i++)
        {
            if (algo[i].name == 1)
            {
                FCFSvisualizer(processes, lastInstance, processNum);
                printf("\n\n");
            }
            else if (algo[i].name == 2)
            {
                RoundRobinVisualizer(processes, algo[i].q, lastInstance, processNum);
            }
            else if (algo[i].name == 4)
            {
                SRTVisualizer(processes, lastInstance, processNum);
            }
            else if (algo[i].name == 5)
            {
                HRRNVisualizer(processes, lastInstance, processNum);
            }
            else if (algo[i].name == 3)
            {
                SPNvisualizer(processes, lastInstance, processNum);
                printf("\n\n");
            }
            else if (algo[i].name == 6)
            {
                FBvisualizer(processes, 1, lastInstance, processNum);
                printf("\n\n");
            }
            else if (algo[i].name == 7)
            {
                FBvisualizer(processes, 0, lastInstance, processNum);
                printf("\n\n");
            }
            else if (algo[i].name == 8)
            {
                AgingVisualizer(processes, algo[i].q, lastInstance, processNum);
                printf("\n\n");
            }
        }
    }
    else if (strcmp(status, "stats") == 0)
    {
        // for loop hena l more than one algorithm
        for (int i = 0; i < countAlgo; i++)
        {
            if (algo[i].name == 1)
            {
                FCFSstatistics(processes, status, processNum, lastInstance);
                printf("\n\n");
            }
            else if (algo[i].name == 2)
            {
                RRstatistics(processes, algo[i].q, processNum);
            }
            else if (algo[i].name == 4)
            {
                SRTstatistics(processes, status, processNum, lastInstance);
            }

            else if (algo[i].name == 5)
            {
                HRRNstatistics(processes, status, processNum, lastInstance);
                // HRRNstats(processes, lastInstance, processNum);
            }
            else if (algo[i].name == 3)
            {
                SPNstatistics(processes, status, processNum, lastInstance);
                printf("\n\n");
            }
            else if (algo[i].name == 6)
            {
                FBstatistics(processes, 1, lastInstance, processNum);
                printf("\n\n");
            }
            else if (algo[i].name == 7)
            {
                FBstatistics(processes, 0, lastInstance, processNum);
                printf("\n\n");
            }
        }
    }
    return 0;
}