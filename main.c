#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int argOption, quantum;
    char* fileName;
    char* memoryType;
    while((argOption = getopt(argc, argv, "f:q:m:")) != -1) {
        switch(argOption) {
            case 'f': //file_name
                fileName = optarg;
                break;
            case 'q': // quantum
                quantum = strtol(optarg, NULL, 10);
                break;
            case 'm': //memory
                memoryType = optarg;
                break;
            default:
                break;
        }
    }
    printf("Quantum is %d\n", quantum);
    printf("File name is %s\n", fileName);
    printf("Memory type is %s\n", memoryType);


    //load all process into ready queue
    return 0;
}
