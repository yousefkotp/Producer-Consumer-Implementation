#ifndef MYSEMAPHORE_H_INCLUDED
#define MYSEMAPHORE_H_INCLUDED
#include <sys/sem.h>

struct sembuf signalSemaphore(int index){
    struct sembuf sem_op;
    sem_op.sem_num = index;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    return sem_op;
}
struct sembuf waitSemaphore(int index){
    struct sembuf sem_op;
    sem_op.sem_num = index;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    return sem_op;
}



#endif