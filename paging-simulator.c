#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int readMultipleOfTwo(char *message)
{
    int value;
    printf(message);
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

void showMenu()
{
    int option;

    printf("1 - Create a new process.\n");
    printf("2 - Show memory status.\n");
    printf("3 - Show pages table.\n");

    printf("\nType the number of the action and press enter: ");
    scanf("%d", &option);

    if (option < 1 && option > 2)
    {
        printf("Invalid option, try again!\n");
        return showMenu();
    }

    switch (option)
    {
    case 1:
        printf("Creating a process\n");
        break;
    case 2:
        printf("Showing memory status\n");
        break;
    case 3:
        printf("Showing pages table\n");
        break;
    }
}

int main()
{
    int memorySize = readMultipleOfTwo("Input the physical memory size: ");
    int processMaxSize = readMultipleOfTwo("Input the processes maximun size: ");
    int pageMaxSize = checkPageSizeIsValid(readMultipleOfTwo("Input the page maximun size: "), memorySize);
    int physicalMemory[memorySize];

    printf("The size of the physical memory is %d, the processes maximun size is  %d, and the pages maximum size is %d what do you thant to do next?\n\n", memorySize, processMaxSize, pageMaxSize);
    showMenu();
}