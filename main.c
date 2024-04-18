#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "process.h"
#include "memScheduler.h"
#include "main.h"




int main(const int argc, char **argv) {
    int argOption;
    const char *fileName = 0;
    memoryType_t memoryType = INVALID;
    int quantum = -1;
    // parse input
    while((argOption = getopt(argc, argv, "f:q:m:")) != -1) {
        switch(argOption) {
            case 'f': //file_name
                fileName = optarg;
                break;
            case 'q': //quantum
                quantum = (int)strtol(optarg, NULL, BASE_10);
                if(quantum < 1 || quantum > 3){
                    printf("Invalid quantum value\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'm': //memory_type
                memoryType = parseMemoryType(optarg);
                if (memoryType == INVALID) {
                    printf("Invalid memory type\n");
                    return EXIT_FAILURE;
                }
                break;
            default:
                break;
        }
    }
    FILE* fp = fopen(fileName, "r");
    if(fp == NULL){
        printf("Failed to obtain file pointer\n");
        return EXIT_FAILURE;
    }
    char* line = NULL;
    size_t len = 0;
    processLL_t* process_list = createLL();
    // read file into process list
    while(getline(&line, &len, fp) != -1){
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
        process->memory.status = UNALLOCATED;
        process->memory.start = -1;
        process->memory.end = -1;
        process->pagedMemory.status = UNALLOCATED;
        process->pagedMemory.pageCount = 0;
        for(int i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++){
            process->pagedMemory.pages[i] = 0;
        }
        addNodeToEnd(process, process_list);
    }
    free(line);
    fclose(fp);
    switch (memoryType) {
        case INFINITE:
        case FIRST_FIT:
        case PAGED:
        case VIRTUAL:
            rrMemoryScheduler(process_list, quantum, memoryType);
            break;
        default:
            printf("Unsupported memory type\n");
            break;
    }
    destroyLL(process_list);
    return EXIT_SUCCESS;
}
memoryType_t parseMemoryType(const char* optarg) {
    if (strcmp(optarg, "infinite") == 0) return INFINITE;
    if (strcmp(optarg, "first-fit") == 0) return FIRST_FIT;
    if (strcmp(optarg, "paged") == 0) return PAGED;
    if (strcmp(optarg, "virtual") == 0) return VIRTUAL;
    return INVALID;
}
