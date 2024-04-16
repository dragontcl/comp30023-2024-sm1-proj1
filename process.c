//
// Created by dragontang on 17/04/24.
//

#include "process.h"
#include <stdio.h>
#include <stdlib.h>

processLL_t *createLL() {
    processLL_t* ll = malloc(sizeof(processLL_t));
    ll->head = NULL;
    ll->tail = NULL;
    return ll;
}
void addNodeToStart(process_t *process, processLL_t *list) {

}

void addNodeToEnd(process_t *process, processLL_t *list) {
    processNode_t *node = malloc(sizeof(processNode_t));//create a node
    node->process = process;
    node->next = NULL;
    if(list->head == NULL) {
        list->head = node;
        list->tail = node;
    }
    else {
        list->tail->next = node;
        list->tail = node;
    }
}
void moveNodeToEnd(process_t *process, processLL_t *list) {

}
void removeNode(process_t *process, processLL_t *list) {

}

void printLL(processLL_t *list) {
    processNode_t *currentNode = list->head;
    while(currentNode != NULL) {
        printf("Process Name: %s | ", currentNode->process->name);
        printf("Arrival Time: %d | ", currentNode->process->arrivalTime);
        printf("Run Time: %d | ", currentNode->process->runTime);
        printf("Completed Time: %d | ", currentNode->process->completedTime);
        printf("Memory Size: %d KB\n", currentNode->process->memorySize);
        currentNode = currentNode->next;
    }
}
void destroyLL(processLL_t *list) {

}