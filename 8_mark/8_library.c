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

int books_num;

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

    books_num = atoi(argv[1]);
    for (int i = 0;  i < books_num; ++i) {
        if ((sem_books[i] = semget(1234 * (i + 1), 1, IPC_CREAT | 0666)) == -1) {
            perror("semget");
            exit(-1);
        }
        if (semctl(sem_books[i], 0, SETVAL, 1) == -1) {
            perror("semctl");
            exit(-1);
        }
    }

    usleep(1000000000);

    clean_all();
    return 0;
}
