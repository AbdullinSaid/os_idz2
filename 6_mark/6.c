#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define MAX_BOOKS 50

int sem_books[MAX_BOOKS];

int books_num, readers_num;

struct sembuf sem_wait = {0, -1, SEM_UNDO};
struct sembuf sem_signal = {0, 1, SEM_UNDO};

void clean_all() {
    for (int i = 0; i < books_num; ++i) {
         semctl(sem_books[i], 0, IPC_RMID, 0);
    }
}

void sigint_handler(int signum) {
    clean_all();
    exit(-1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    scanf("%d %d", &books_num, &readers_num);
    for (int i = 0; i < books_num; ++i) {
        if ((sem_books[i] = semget(1234 * (i + 1), 1, IPC_CREAT | 0666)) == -1) {
            perror("semget");
            exit(-1);
        }
        if (semctl(sem_books[i], 0, SETVAL, 1) == -1) {
            perror("semctl");
            exit(-1);
        }
    }

    for (int i = 0; i < readers_num; ++i) {
        int pid = fork();
        if (pid < 0) {
            printf("Error creating process\n");
            return 1;
        } else if (pid == 0) {
            srand(1234 * (i + 1));
            for (int j = 0; j < 5; ++j) {
                int ind = rand() % books_num;
                printf("Reader %d trying to take book %d\n", i, ind);
                semop(sem_books[ind], &sem_wait, 1);
                printf("Reader %d took book %d\n", i, ind);
                usleep(100000);
                printf("Reader %d returned book %d\n", i, ind);
                semop(sem_books[ind], &sem_signal, 1);
            }
        }
    }

    for (int i = 0; i < readers_num; ++i) {
        wait(NULL);
    }

    clean_all();
    return 0;
}
