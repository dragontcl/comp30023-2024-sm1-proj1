//
// Created by dragontang on 17/04/24.
//

#ifndef PROCESS_H
#define PROCESS_H
#define MAX_NAME_LENGTH 9

typedef enum processState{
  READY,
  RUNNING,
  FINISHED
} processState_t;

typedef enum memoryStatus{
  ALLOCATED,
  UNALLOCATED
} memoryStatus_t;

typedef struct memory {
  int start;
  int end;
  memoryStatus_t status;
} memory_t;

typedef struct process{
  int arrivalTime;
  char name [MAX_NAME_LENGTH];
  int runTime;
  int remainingTime;
  int completedTime;
  int memorySize; //in KB
  memory_t memory;
  processState_t status;
} process_t;

typedef struct processNode {
  process_t *process;
  struct processNode *next;
} processNode_t;

typedef struct processList {
  processNode_t *head;
  processNode_t *tail;
} processLL_t;

processLL_t* createLL();
void addNodeToStart(process_t *process, processLL_t *list);
void addNodeToEnd(process_t *process, processLL_t *list);
void moveNodeToEnd(const process_t *process, processLL_t *list);
void removeNode(const process_t *process, processLL_t *list);
void printLL(const processLL_t *list);
void destroyLL(processLL_t *list);
int isEmpty(const processLL_t *list);

#endif //PROCESS_H
