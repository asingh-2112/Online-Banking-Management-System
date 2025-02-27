#ifndef COMMON
#define COMMON

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


int init_semphore(int ID, char *file_name) {
    key_t semKey = ftok(file_name, ID);
    union semun {
        int val;
    } semSet;

    int semctlStatus;
    int sem_id = semget(semKey, 1, 0);
    if (sem_id == -1) {
        sem_id = semget(semKey, 1, IPC_CREAT | 0700);
        if (sem_id == -1) {
            perror("Error while creating semaphore!");
            _exit(1);
        }

        semSet.val = 1;
        semctlStatus = semctl(sem_id, 0, SETVAL, semSet);
        if (semctlStatus == -1) {
            perror("Error while initializing a binary sempahore!");
            _exit(1);
        }
    }
    return sem_id;
}

bool lock(struct sembuf *semOp, int sem_id) {
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(sem_id, semOp, 1);
    if (semopStatus == -1) {
        perror("Error while locking critical section");
        return false;
    }
    return true;
}

bool unlock(struct sembuf *semOp, int sem_id) {
    semOp->sem_op = 1;
    int semopStatus = semop(sem_id, semOp, 1);
    if (semopStatus == -1) {
        perror("Error while operating on semaphore!");
        _exit(1);
    }
    return true;
}

#endif