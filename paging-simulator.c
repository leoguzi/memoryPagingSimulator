#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    int *pageTable;
    int processSize;
    int numPages;
} PageTable;

typedef struct
{
    unsigned char *memory;
    bool *freeFrames;
    int totalFrames;
    int frameSize;
    int freeFrameCount;
} PhysicalMemory;

typedef struct
{
    int processId;
    int size;
    PageTable *pageTable;
} Process;

int readMultipleOfTwo(char *message)
{
    int value;
    printf("%s", message);
    scanf("%d", &value);
    if (value % 2 == 0 && value != 0)
    {
        return value;
    }
    printf("\nValue must be multiple of 2 and greather than 0! \n\n");
    readMultipleOfTwo(message);
}

int checkPageSizeIsValid(int pageSize, int memorySize)
{
    if (pageSize > memorySize)
    {
        printf("The page size must be less than the memory size!\n");
        return readMultipleOfTwo("Input the page maximun size: ");
    }
    return pageSize;
}

void initPhysicalMemory(PhysicalMemory *pm, int memorySize, int frameSize)
{
    pm->memory = (unsigned char *)malloc(memorySize * sizeof(unsigned char));
    pm->frameSize = frameSize;
    pm->totalFrames = memorySize / frameSize;
    pm->freeFrames = (bool *)malloc(pm->totalFrames * sizeof(bool));
    pm->freeFrameCount = pm->totalFrames;

    for (int i = 0; i < pm->totalFrames; i++)
    {
        pm->freeFrames[i] = true;
    }

    memset(pm->memory, 0, memorySize);
}

void freePhysicalMemory(PhysicalMemory *pm)
{
    free(pm->memory);
    free(pm->freeFrames);
    printf("Physical memory freed\n");
}

void viewPhysicaMemory(PhysicalMemory *pm)
{
    printf("Physical memory: %d%% free\n", (pm->freeFrameCount * 100) / pm->totalFrames);

    for (int i = 0; i < pm->totalFrames; i++)
    {
        printf("Frame %d: ", i);
        if (pm->freeFrames[i])
        {
            printf("Free\n");
        }
        else
        {
            for (int j = 0; j < pm->frameSize; j++)
            {
                printf("%02X ", pm->memory[i * pm->frameSize + j]);
            }
            printf("\n");
        }
    }
    printf("\n");
}

void viewPageTable(Process *process)
{
    printf("/n");
    printf("Page table for process %d:\n", process->processId);
    printf("Process size: %d bytes\n", process->size);
    printf("Pages number: %d\n\n", process->pageTable->numPages);

    for (int i = 0; i < process->pageTable->numPages; i++)
    {
        printf("Page %d -> Frame %d\n", i, process->pageTable->pageTable[i]);
    }
    printf("\n");
}

Process *createProcess(int processId, int processSize, int maxProcessSize, PhysicalMemory *pm)
{
    if (processSize > maxProcessSize)
    {
        printf("\nError: the process must be smaller than %d.\n\n", maxProcessSize);
        return NULL;
    }

    int numPages = (processSize + pm->frameSize - 1) / pm->frameSize;

    if (numPages > pm->freeFrameCount)
    {
        printf("Error: not enougth memory to start the process.\n\n ");
        return NULL;
    }

    Process *process = (Process *)malloc(sizeof(Process));
    process->processId = processId;
    process->size = processSize;
    process->pageTable = (PageTable *)malloc(sizeof(PageTable));
    process->pageTable->numPages = numPages;
    process->pageTable->pageTable = (int *)malloc(numPages * sizeof(int));

    int allocatedPages = 0;
    for (int i = 0; i < pm->totalFrames && allocatedPages < numPages; i++)
    {
        if (pm->freeFrames[i])
        {
            pm->freeFrames[i] = false;
            pm->freeFrameCount--;
            process->pageTable->pageTable[allocatedPages] = i;
            allocatedPages++;
        }
    }

    for (int i = 0; i < processSize; i++)
    {
        pm->memory[process->pageTable->pageTable[i / pm->frameSize] * pm->frameSize + (i % pm->frameSize)] = rand() % 256;
    }

    return process;
}

int getProcessId(int processCount, Process *processList[])
{
    int processId;
    printf("Type the new process ID: ");
    scanf("%d", &processId);

    if (processId < 0)
    {
        printf("ID should be a number greather than 0. \n\n");
        return getProcessId(processCount, processList);
    };
    for (int i = 0; i < processCount; i++)
    {

        if (processList[i]->processId == processId)
        {
            printf("This ID is already taken by another process!\n\n");
            return getProcessId(processCount, processList);
        }
    }
    return processId;
}

int getExistentProcessId(int processCount, Process *processList[])
{
    int processId;
    bool found = false;
    while (!found)
    {
        printf("Type the process ID (-1 to go back): ");
        scanf("%d", &processId);
        if (processId == -1)
        {
            break;
        }

        for (int i = 0; i < processCount; i++)
        {
            if (processList[i]->processId == processId)
            {
                found = true;
                return processId;
            }
        }
        printf("\nNo process found with the provided ID :(\n\n");
        return getExistentProcessId(processCount, processList);
    }
}

void showMenu(PhysicalMemory *pm, int memorySize, int processMaxSize, int pageMaxSize)
{
    int option;
    Process *processList[100];
    int processCount = 0;
    int processId = 0;

    while (true)
    {
        printf("The size of the physical memory is %d bytes, the processes maximun size is  %d bytes "
               "and the pages maximum size is %d bytes.\nWhat do you want to do next?\n\n",
               memorySize, processMaxSize, pageMaxSize);

        printf("1 - Create a new process.\n");
        printf("2 - Show memory status.\n");
        printf("3 - Show pages table.\n");
        printf("4 - Exit.\n");

        printf("\nType the number of the action and press enter: ");
        scanf("%d", &option);

        if (option == 1)
        {
            int processSize;
            processId = getProcessId(processCount, processList);
            printf("Type the process size (bytes): ");
            scanf("%d", &processSize);

            Process *newProcess = createProcess(processId, processSize, processMaxSize, pm);
            if (newProcess != NULL)
            {
                processList[processCount++] = newProcess;
                printf("Process %d sucessfuly created.\n\n", processId);
            }
        }
        else if (option == 2)
        {
            printf("Showing memory status...\n\n");
            viewPhysicaMemory(pm);
        }
        else if (option == 3)
        {
            if (processCount == 0)
            {
                printf("\nThere are no processes in the memory! \n\n");
                continue;
            }

            processId = getExistentProcessId(processCount, processList);

            for (int i = 0; i < processCount; i++)
            {
                if (processList[i]->processId == processId)
                {
                    viewPageTable(processList[i]);
                    break;
                }
            }
        }
        else if (option == 4)
        {
            printf("Exiting\n");
            break;
        }
        else
        {
            printf("Invalid option, try again!\n");
        }
    }
}

int main()
{
    int memorySize = readMultipleOfTwo("Input the physical memory size: ");
    int processMaxSize = readMultipleOfTwo("Input the processes maximun size: ");
    int pageMaxSize = checkPageSizeIsValid(readMultipleOfTwo("Input the page maximun size: "), memorySize);
    int physicalMemory[memorySize];

    PhysicalMemory pm;

    initPhysicalMemory(&pm, memorySize, pageMaxSize);

    showMenu(&pm, memorySize, processMaxSize, pageMaxSize);

    freePhysicalMemory(&pm);
}