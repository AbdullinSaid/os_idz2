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

struct sembuf sem_wait = {0, -1, SEM_UNDO};
struct sembuf sem_signal = {0, 1, SEM_UNDO};

int main(int argc, char *argv[]) {
    books_num = atoi(argv[1]);
    for (int i = 0; i < books_num; ++i) {
        if ((sem_books[i] = semget(1234 * (i + 1), 1, IPC_CREAT | 0666)) == -1) {
            perror("semget");
            exit(-1);
        }
    }

    int i = atoi(argv[2]);

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
    return 0;
}
