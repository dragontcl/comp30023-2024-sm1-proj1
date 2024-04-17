#include "memScheduler.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXIMUM_MEMORY 2048
#define PAGE_SIZE 4

int findProcessAndInsertAtEnd(processLL_t *process_list, processLL_t *running_list, const int clock, const int quantum) {
    int found = 0;
    const processNode_t *currentNode = process_list->head;
    while(currentNode != NULL) {
        const processNode_t *nextNode = currentNode->next;
        if(currentNode->process->arrivalTime <= clock+quantum) {
            found = 1;
            addNodeToEnd(currentNode->process, running_list);
            removeNode(currentNode->process, process_list);
        }
        currentNode = nextNode;
    }
    return found;
}
int turnAroundTime(const process_t *finishedProcesses) {
    return finishedProcesses->completedTime - finishedProcesses->arrivalTime;
}
double avgOverheadTime(const processLL_t *finishedProcesses){
        double totalOverheadTime = 0;
        int totalProcesses = 0;
        const processNode_t *currentNode = finishedProcesses->head;
        while (currentNode != NULL) {
                totalOverheadTime += ((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime);
                totalProcesses++;
                currentNode = currentNode->next;
        }
        return round(totalOverheadTime / totalProcesses * 100) / 100;
}
double maxOverheadTime(const processLL_t *finishedProcesses){
    double maxOverheadTime = 0;
    const processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        if(((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime) > maxOverheadTime){
            maxOverheadTime = ((double)turnAroundTime(currentNode->process)/ currentNode->process->runTime);
        }
        currentNode = currentNode->next;
    }

    return round(maxOverheadTime * 100) / 100;
}
int avgTurnAroundTime(const processLL_t *finishedProcesses) {
    int totalTurnAroundTime = 0;
    int totalProcesses = 0;
    const processNode_t *currentNode = finishedProcesses->head;
    while (currentNode != NULL) {
        totalTurnAroundTime += turnAroundTime(currentNode->process);
        totalProcesses++;
        currentNode = currentNode->next;
    }
    return (int)ceil((double)totalTurnAroundTime / totalProcesses);
}
void rrInfiniteMem(processLL_t *process_list, const int quantum) {
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    const process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(process_list) || !isEmpty(runningProcesses)) {
        //if(findProcessAndInsert(process_list, runningProcesses, clock)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        //}
        if(findProcessAndInsertAtEnd(process_list, runningProcesses, clock, quantum)) {
            //printf("NEW PROCESS GOT INSERTED\n");
        }
        const int timeSlice = quantum; // Always use full quantum time
        // there are processes that can be ran in the round robin scheduler
        if (!isEmpty(runningProcesses)) {
            // get the first process in the list
            const processNode_t *currentNode = runningProcesses->head;
            // ** BUG FIX FOR QUEUE ISSUE **
            while (currentNode != NULL && currentNode->process->arrivalTime > clock) {
                currentNode = currentNode->next;  // Skip processes not yet arrived
            }
            if (currentNode == NULL) {
                clock += timeSlice;
                continue;
            }
            // ** BUG FIX FOR QUEUE ISSUE **
            currentNode->process->status = RUNNING; // allow process to run
            // print on context switch
            if(currentNode->process->status == RUNNING) {
                if (lastRunningProcess != currentNode->process) {
                    printf("%d,RUNNING,process-name=%s,remaining-time=%d\n",
                           clock, currentNode->process->name, currentNode->process->remainingTime);
                    lastRunningProcess = currentNode->process;
                }
                currentNode->process->remainingTime -= timeSlice;
            }
            // check if process is finished && is running
            if (currentNode->process->remainingTime <= 0 && currentNode->process->status == RUNNING) {
                currentNode->process->completedTime = clock + timeSlice;
                printf("%d,FINISHED,process-name=%s,proc-remaining=",
                       currentNode->process->completedTime, currentNode->process->name);
                addNodeToEnd(currentNode->process, finishedProcesses);
                removeNode(currentNode->process, runningProcesses);
                lastRunningProcess = NULL;
                int i = 0;
                const processNode_t *temp = runningProcesses->head;
                while (temp != NULL) {
                    i++;
                    temp = temp->next;
                }
                printf("%d\n", i);
            } else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
            clock += timeSlice;
        } else {
            clock+=timeSlice;;
        }
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", maxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}
int firstFitAllocate(process_t *process, int memory[]) {
    const int memoryRequired = process->memorySize;
    if (process->memory.status == ALLOCATED) { // already allocated memory no need to allocate again
        return 1;
    }
    for (int i = 0; i < MAXIMUM_MEMORY; i++) {
        if (memory[i] == 0) {
            int block = 0;
            for (int j = i; j < MAXIMUM_MEMORY; j++) {
                if (memory[j] == 0) {
                    block++;
                } else {
                    break;
                }
            }
            if (block >= memoryRequired) {
                for (int j = i; j < i + memoryRequired; j++) {
                    memory[j] = 1;
                }
                process->memory.start = i;
                process->memory.end = i + memoryRequired - 1;
                process->memory.status = ALLOCATED;
                return 1; // successfully allocated memory
            }
        }
    }
    return 0; // failed to allocate memory
}
void firstFitDeallocate(process_t *process, int memory[]) {
    if(process->memory.status == UNALLOCATED) {
        return;
        // already deallocated memory no need to deallocate again
    }
    for(int i = process->memory.start; i <= process->memory.end; i++) {
        memory[i] = 0;
    }
    process->memory.start = -1;
    process->memory.end = -1;
    process->memory.status = UNALLOCATED;
}
int calculateUsedMemory(int memory[], const memoryType_t type) {
    int usedMemory = 0;
    int maxMemory = 0;
    switch (type){
        case FIRST_FIT:
            maxMemory = MAXIMUM_MEMORY;
            break;
        case PAGED:
            maxMemory = MAXIMUM_MEMORY/PAGE_SIZE;
            break;
        default:
            break;
        }
    for (int i = 0; i < maxMemory; i++) {
        if (memory[i] == 1) {
            usedMemory++;
        }
    }
    double percentage = ((double) usedMemory / maxMemory) * 100.0;
    return (int)ceil(percentage); //do some casting magic to round
}
void rrFirstFitMem(processLL_t *process_list, const int quantum) {
    int memory[MAXIMUM_MEMORY] = {0};
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    const process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(process_list) || !isEmpty(runningProcesses)) {

        if(findProcessAndInsertAtEnd(process_list, runningProcesses, clock, quantum)) {
            //printf("NEW PROCESS GOT INSERTED AT BACK\n");
        }
        const int timeSlice = quantum; // Always use full quantum time
        // there are processes that can be ran in the round robin scheduler
        if (!isEmpty(runningProcesses)) {
            // get the first process in the list
            const processNode_t *currentNode = runningProcesses->head;
            // ** BUG FIX FOR QUEUE ISSUE **
            while (currentNode != NULL && currentNode->process->arrivalTime > clock) {
                currentNode = currentNode->next;  // Skip processes not yet arrived
            }
            if (currentNode == NULL) {
                clock += timeSlice;
                continue;
            }
            // ** BUG FIX FOR QUEUE ISSUE **
            if(firstFitAllocate(currentNode->process, memory)) {

                currentNode->process->status = RUNNING; // allow process to run
            }
            else {
                moveNodeToEnd(currentNode->process, runningProcesses);
                lastRunningProcess = currentNode->process;
                //currentNode = currentNode->next;
                continue; // allocation failed, try next process
            }
            // print on context switch
            if(currentNode->process->status == RUNNING) {
                if (lastRunningProcess != currentNode->process) {
                    printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,allocated-at=%d\n",
                           clock, currentNode->process->name, currentNode->process->remainingTime,
                           calculateUsedMemory(memory, FIRST_FIT),
                           currentNode->process->memory.start);
                    lastRunningProcess = currentNode->process;
                }
                currentNode->process->remainingTime -= timeSlice;
            }
            // check if process is finished && is running
            if (currentNode->process->remainingTime <= 0 && currentNode->process->status == RUNNING) {
                currentNode->process->completedTime = clock + timeSlice;
                printf("%d,FINISHED,process-name=%s,proc-remaining=",
                       currentNode->process->completedTime, currentNode->process->name);
                firstFitDeallocate(currentNode->process, memory);
                addNodeToEnd(currentNode->process, finishedProcesses);
                removeNode(currentNode->process, runningProcesses);
                lastRunningProcess = NULL;
                int i = 0;
                const processNode_t *temp = runningProcesses->head;
                while (temp != NULL) {
                    i++;
                    temp = temp->next;
                }
                printf("%d\n", i);
            } else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
        }
        clock += timeSlice;
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", maxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}
int pagedMemAllocate(process_t *process, int memory[]) {
    if (process->paged_memory.status == ALLOCATED) { // already allocated memory no need to allocate again
        return 1;
    }
    const int pagesRequired = (process->memorySize/PAGE_SIZE)+(process->memorySize%PAGE_SIZE);
    int unallocatedPages = 0;
    for (int i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++) {
        if (memory[i] == 0) {
            unallocatedPages++;
        }
    }
    if (unallocatedPages < pagesRequired) {
        return 0; // not enough memory available
    }
    // Proceed with allocation
    int pagesAllocated = 0;
    for (int i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++) {
        if (memory[i] == 0) {
            memory[i] = 1; // allocate this page to the process
            process->paged_memory.pages[i] = 1; // mark this page as allocated in the process's paged memory
            pagesAllocated++;
            if (pagesAllocated == pagesRequired) {
                process->paged_memory.status = ALLOCATED;
                return 1; // successfully allocated memory
            }
        }
    }
    return 0; // failed to allocate memory
}
void pagedMemDeallocate(process_t *process, int memory[], const int clock) {
    if (process->paged_memory.status == UNALLOCATED) {
        return; // already deallocated memory no need to deallocate again
    }
    printf("%d,EVICTED,evicted-frames=[",clock);
    printMemPage(process);
    printf("]\n");
    for (int i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++) {
        if (process->paged_memory.pages[i] == 1) {
            memory[i] = 0; // deallocate this page from the process
            process->paged_memory.pages[i] = 0; // mark this page as deallocated in the process's paged memory
        }
    }
    process->paged_memory.status = UNALLOCATED;
}
void printMemPage(const process_t * process) {
    int i = 0;
    int finalPage = 0;
    for (i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++) {
        if (process->paged_memory.pages[i] == 1) {
            finalPage = i;
        }
    }
    for (i = 0; i < MAXIMUM_MEMORY/PAGE_SIZE; i++) {
        if (process->paged_memory.pages[i] == 1) {
            if(i != finalPage)
                printf("%d,", i);
            else
                printf("%d", i);
        }
    }
}
void rrPagedMem(processLL_t *process_list, const int quantum) {
    int memory[MAXIMUM_MEMORY] = {0};
    processLL_t *runningProcesses = createLL();
    processLL_t *finishedProcesses = createLL();
    process_t *lastRunningProcess = NULL;
    int clock = 0;
    while (!isEmpty(process_list) || !isEmpty(runningProcesses)) {

        if(findProcessAndInsertAtEnd(process_list, runningProcesses, clock, quantum)) {
            //printf("NEW PROCESS GOT INSERTED AT BACK\n");
        }
        const int timeSlice = quantum; // Always use full quantum time
        // there are processes that can be ran in the round robin scheduler
        if (!isEmpty(runningProcesses)) {
            // get the first process in the list
            const processNode_t *currentNode = runningProcesses->head;
            // ** BUG FIX FOR QUEUE ISSUE **
            while (currentNode != NULL && currentNode->process->arrivalTime > clock) {
                currentNode = currentNode->next;  // Skip processes not yet arrived
            }
            if (currentNode == NULL) {
                clock += timeSlice;
                continue;
            }
            // ** BUG FIX FOR QUEUE ISSUE **
            if(pagedMemAllocate(currentNode->process, memory)) {

                currentNode->process->status = RUNNING; // allow process to run
            }
            else {
                //moveNodeToEnd(currentNode->process, runningProcesses);
                //lastRunningProcess = currentNode->process;
                //currentNode = currentNode->next;
                pagedMemDeallocate(lastRunningProcess, memory, clock);
                continue; // allocation failed, try next process
            }
            // print on context switch
            if(currentNode->process->status == RUNNING) {
                if (lastRunningProcess != currentNode->process) {
                    printf("%d,RUNNING,process-name=%s,remaining-time=%d,mem-usage=%d%%,mem-frames=[",
                           clock, currentNode->process->name, currentNode->process->remainingTime,
                           calculateUsedMemory(memory, PAGED));
                    printMemPage(currentNode->process);
                    printf("]\n");

                    lastRunningProcess = currentNode->process;
                }
                currentNode->process->remainingTime -= timeSlice;
            }
            // check if process is finished && is running
            if (currentNode->process->remainingTime <= 0 && currentNode->process->status == RUNNING) {
                currentNode->process->completedTime = clock + timeSlice;
                pagedMemDeallocate(currentNode->process, memory, currentNode->process->completedTime);

                printf("%d,FINISHED,process-name=%s,proc-remaining=",
                       currentNode->process->completedTime, currentNode->process->name);
                addNodeToEnd(currentNode->process, finishedProcesses);
                removeNode(currentNode->process, runningProcesses);
                lastRunningProcess = NULL;
                int i = 0;
                const processNode_t *temp = runningProcesses->head;
                while (temp != NULL) {
                    i++;
                    temp = temp->next;
                }
                printf("%d\n", i);
            } else {
                moveNodeToEnd(currentNode->process, runningProcesses);
            }
        }
        clock += timeSlice;
    }
    printf("Turnaround time %d\n", avgTurnAroundTime(finishedProcesses));
    printf("Time overhead %.2f %.2f\n", maxOverheadTime(finishedProcesses) ,avgOverheadTime(finishedProcesses));
    printf("Makespan %d\n", clock);
    destroyLL(runningProcesses);
    destroyLL(finishedProcesses);
}
void rrVirtualMem(processLL_t *process_list, const int quantum) {

}