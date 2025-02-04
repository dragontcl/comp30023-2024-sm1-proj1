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
    processNode_t *node = malloc(sizeof(processNode_t));//create a node
    node->process = process;
    node->next = list->head;
    list->head = node;
    if(list->tail == NULL) {
        list->tail = node;
    }
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
void moveNodeToEnd(const process_t *process, processLL_t *list) {
        processNode_t *currentNode = list->head;
        processNode_t *previousNode = NULL;
        while(currentNode != NULL) {
                if(currentNode->process == process) {
                if(currentNode == list->tail) {
                        return;
                }
                if(previousNode == NULL) {
                        list->head = currentNode->next;
                }
                else {
                        previousNode->next = currentNode->next;
                }
                list->tail->next = currentNode;
                currentNode->next = NULL;
                list->tail = currentNode;
                return;
                }
                previousNode = currentNode;
                currentNode = currentNode->next;
        }
}
void removeNode(const process_t *process, processLL_t *list) {
    processNode_t *currentNode = list->head;
    processNode_t *previousNode = NULL;
    while(currentNode != NULL) {
        if(currentNode->process == process) {
            if(previousNode == NULL) {
                list->head = currentNode->next;
            }
            else {
                previousNode->next = currentNode->next;
            }
            if(currentNode == list->tail) {
                list->tail = previousNode;
            }
            free(currentNode);
            return;
        }
        previousNode = currentNode;
        currentNode = currentNode->next;
    }
}
void printLL(const processLL_t *list) {
    const processNode_t *currentNode = list->head;
    while(currentNode != NULL) {
        printf("Process Name: %s | Arrival Time: %d | Run Time: %d | Completed Time: %d | Memory Size: %d KB\n",
               currentNode->process->name, currentNode->process->arrivalTime, currentNode->process->runTime,
               currentNode->process->completedTime, currentNode->process->memorySize);
        currentNode = currentNode->next;
    }
}
void destroyLL(processLL_t *list) {
        processNode_t *currentNode = list->head;
        while(currentNode != NULL) {
                processNode_t *nextNode = currentNode->next;
                free(currentNode->process);
                free(currentNode);
                currentNode = nextNode;
        }
        free(list);
}
int isEmpty(const processLL_t *list) {
    return list->head == NULL;
}