#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "process.h"
#include "memScheduler.h"
#define BASE_10 10
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0



int main(int argc, char **argv) {
    int argOption;
    char *fileName = 0;
    char *memoryType = 0;
    int quantum = -1;
    // parse input
    while((argOption = getopt(argc, argv, "f:q:m:")) != -1) {
        switch(argOption) {
            case 'f': //file_name
                fileName = optarg;
                break;
            case 'q': //quantum
                //use strtol to handle input with string and ints
                quantum = (int)strtol(optarg, NULL, BASE_10);
                //quantum limit between 1 and 3
                if(quantum < 1 || quantum > 3){
                    printf("Invalid quantum value\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'm': //memory_type
                memoryType = optarg;
                if(strcmp(optarg, "infinite") != 0 &&
                    strcmp(optarg, "first-fit") != 0 &&
                    strcmp(optarg, "paged") != 0 &&
                    strcmp(optarg, "virtual") != 0){
                    printf("Invalid memory type\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                break;
        }
    }
    FILE* fp = fopen(fileName, "r");
    printf("Quantum is %d\n", quantum);
    printf("File name is %s\n", fileName);
    printf("Memory type is %s\n", memoryType);
    if(fp == NULL){
        printf("Failed to obtain file pointer\n");
        return EXIT_FAILURE;
    }
    char* line = NULL;
    size_t len = 0;
    processLL_t* process_list = createLL();
    // read file into process list
    while(getline(&line, &len, fp) != -1){
        // read line into process
        process_t *process = (process_t *)malloc(sizeof(process_t));
        const char * token = strtok(line, " ");
        process->arrivalTime = (int)strtol(token, NULL, BASE_10);
        token = strtok(NULL, " ");
        strcpy(process->name, token);
        token = strtok(NULL, " ");
        process->runTime = (int)strtol(token, NULL, BASE_10);
        process->remainingTime =  process->runTime;
        token = strtok(NULL, " ");
        process->memorySize = (int)strtol(token, NULL, BASE_10);
        addNodeToEnd(process, process_list);
    }
    free(line);
    if(strcmp(memoryType, "infinite") == 0) {
        //rrInfiniteMem(process_list,quantum);
    }
    if(strcmp(memoryType, "first-fit") == 0) {

    }
    if(strcmp(memoryType, "paged") == 0) {

    }
    if(strcmp(memoryType, "virtual") == 0) {

    }
    destroyLL(process_list);
    return EXIT_SUCCESS;
}
